//ajout du framework mariadb
const mariaDb = require('mariadb')

//ajout du framework fastify
const fastify = require('fastify')({ logger: true })

//ajout du framework fileSystem
const fs = require('fs')

//ajout du framework path
const path = require('path')

//option du midlman fastify/static utiliser pour faire le transfer de fichier entre le server et le client
fastify.register(require('@fastify/static'), {
  root: path.join(__dirname, 'SiteWeb'),
  prefix: '/',
  constraints: {}
})


// Declare la route principale
fastify.get('/', async (req, reply) => {
  return reply.sendFile('PageWeb.html')
});


// Start the server
fastify.listen({ port: 3000, host: '::' }, function (err, address) {
  if (err) {
      fastify.log.error(err)
      process.exit(1)
    }
    fastify.log.info(`server listening on ${address}`)
  });