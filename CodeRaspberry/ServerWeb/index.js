//ajout du framework mariadb
const mariadb = require('mariadb')

//ajout du framework fastify
const fastify = require('fastify')({ logger: true })

//ajout du framework fileSystem
const fs = require('fs')

//ajout du framework path
const path = require('path')


// cree la pool de la base de donnee
const pool = mariadb.createPool({
  host: 'localhost',
  port: '3306',
  user: 'userBERUCHE',
  password: 'BERUCHE'
});

//option du midlman fastify/static utiliser pour faire le transfer de fichier entre le server et le client
fastify.register(require('@fastify/static'), 
{
  root: path.join(__dirname, 'SiteWeb'),
  prefix: '/',
  constraints: {}
})

//reglage de "Access-Control-Allow-Origin" (autorisation a tous le monde de faire de requette au serveur)
fastify.register(require('@fastify/cors'), { 
  origin: '*'
})


// Start the server
fastify.listen({ port: 3000, host: '::' }, function (err, address) 
{
  if (err) 
  {
    fastify.log.error(err)
    process.exit(1)
  }
});

// Declare la route principale
fastify.get('/', async (req, reply) =>
{
  return reply.sendFile('PageWeb.html')
});

//Declare une nouvelle route secondaire
fastify.get('/lastLigneDB', async (req, reply) => 
{
   await lastValDataBase().then(function(data) 
   {
    reply.header('Content-Type', 'application/json')
    return reply.send(data)
   })
});


//-------------------------------------Fonction de maraiDB------------------------------------- 
async function lastValDataBase() 
{
  /*
    Fonction de recuperation de la derniere ligne de valeur dans la base de donnee mariadB

    return:
      lastRow(obj): retourne l'objet de la dernière ligne de la base de donnee
  */
 
  //connection a la db
  let conn = await pool.getConnection();

  //demande de valeur via requet sql
  conn.query("USE BERUCHE");
  const result = await conn.query("SELECT * FROM Mesure ORDER BY DateHeure DESC LIMIT 1");

  //convertion des valeurs
  const lastRow = result[0]
  
  //deconnection de la db
  conn.release();

  return lastRow;
}