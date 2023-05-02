//ajout du framework mariadb
const mariadb = require('mariadb')

//ajout du framework fastify
const fastify = require('fastify')({ logger: true })

//ajout du framework fileSystem
const fs = require('fs')

//ajout du framework path
const path = require('path')

//cree la pool de la base de donnee
var pool = mariadb.createPool({
  host: 'localhost',
  port: '3306',
  user: 'userBERUCHE',
  password: 'BERUCHE'
});

lastValDataBase();



//-------------------------------------Fonction de maraiDB------------------------------------- 
async function lastValDataBase() 
{
  let conn;
  try 
  {
    conn = await pool.getConnection();
    
    conn.query("USE BERUCHE");
    const rows = await conn.query("SELECT * FROM Mesure ORDER BY DateHeure DESC LIMIT 1");

    for (i = 0, len = rows.length; i < len; i++)
    {
      console.log(`Date:${rows[i].DateHeure} Ruche:${rows[i].Ruche} TempInter:${rows[i].TempInter} HumidInter:${rows[i].HumidInter} TempExter:${rows[i].TempExter} HumiExter:${rows[i].HumiExter} Masse:${rows[i].Masse} Proxi:${rows[i].Proxi}`);
    }

  }finally
  {
  if (conn) conn.release();
  }
}






//-------------------------------------Fonction de fastify------------------------------------- 
// //option du midlman fastify/static utiliser pour faire le transfer de fichier entre le server et le client
// fastify.register(require('@fastify/static'), {
//   root: path.join(__dirname, 'SiteWeb'),
//   prefix: '/',
//   constraints: {}
// })

// // Start the server
// fastify.listen({ port: 3000, host: '::' }, function (err, address) {
//   if (err) {
//       fastify.log.error(err)
//       process.exit(1)
//     }
//     fastify.log.info(`server listening on ${address}`)
//   });

// // Declare la route principale
// fastify.get('/', async (req, reply) => {
//   return reply.sendFile('PageWeb.html')
// });
