window.onload=function() {   
    //fonction de recuperation et d affichage des derniere valeurs recu de la ruche
    async  function fetchData()
    { 
        /*
            fonction de recuperation et d affichage des valeurs des dernieres valeurs recu par la bdd

        */

        //recuperation des valeurs
        let url = 'http://192.168.1.93:3000/lastLigneDB'
        const data = await fetch(url).then(res => res.json())

        //affichage des valeurs
        document.getElementById('TempInter').innerHTML = data.TempInter + "°C"
        document.getElementById('HumidInter').innerHTML = data.HumidInter + "%"
        document.getElementById('TempExter').innerHTML = data.TempExter + "°C"
        document.getElementById('HumidExter').innerHTML = data.HumiExter + "%"
        document.getElementById('Masse').innerHTML = data.Masse + "g"
        document.getElementById('Proxi').innerHTML = data.Proxi
        document.getElementById('Vbat').innerHTML = data.Vbat + "V"
    }

    
    fetchData()
    //repete la fonction fetchData() toutes les secondes
    setInterval(function() {fetchData();}, 1000);
}