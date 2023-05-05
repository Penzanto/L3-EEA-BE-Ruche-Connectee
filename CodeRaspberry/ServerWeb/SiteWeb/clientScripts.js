window.onload=function() {

    async function fetchData()
    {
        const data = await fetch('http://localhost:3000/exemple')
        console.log(data)
    }

    fetchData()
    





    var VarTest = "123"
    var SHT31_Temp = VarTest
    var SHT31_Humid = VarTest
    var DHT11_Temp = VarTest
    var DHT11_Humid = VarTest
    var cap_Dist = VarTest
    var cap_Masse = VarTest

    //lien des variables avec ce qui est affiche sur la page html
    document.getElementById('SHT31_Temp').innerHTML = SHT31_Temp
    document.getElementById('SHT31_Humid').innerHTML = SHT31_Humid
    //
    document.getElementById('DHT11_Temp').innerHTML = DHT11_Temp
    document.getElementById('DHT11_Humid').innerHTML = DHT11_Humid
    //
    document.getElementById('Distance').innerHTML = cap_Dist
    //
    document.getElementById('Masse').innerHTML = cap_Masse
}