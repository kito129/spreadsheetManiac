<?php

foreach ($_REQUEST as $tipo_dato => $value)
{
	if ($tipo_dato == "temperatura") {
		$temperatura = $value;
		$umidita = "NULL";
		$pressione = "NULL";
		$luce = "NULL";
		$battery = "NULL";
	}
	else if ($tipo_dato == "umidita") {
		$temperatura = "NULL";
		$umidita = $value;
		$pressione = "NULL";
		$luce = "NULL";
		$battery = "NULL";
	}
	else if ($tipo_dato == "pressione") {
		$temperatura = "NULL";
		$umidita = "NULL";
		$pressione = $value;
		$luce = "NULL";
		$battery = "NULL";
	}
	else if ($tipo_dato == "luce") {
		$temperatura = "NULL";
		$umidita = "NULL";
		$battery = "NULL";
		$pressione = "NULL";
		$luce = $value;
	}
	else if ($tipo_dato == "battery"){
		$temperatura = "NULL";
		$umidita = "NULL";
		$pressione = "NULL";
		$luce = "NULL";
		$battery = $value;
	}
}

//Le tue impostazioni database - TODO
$username="user";
$password="password";
$database="database";
$tabella= "meteo";
$host="host";


//Connetti al database
if ($con = mysqli_connect($host, $username, $password))
  {
  	mysqli_select_db($con, $database) or die ("Impossibile selezionare database.");

    /*
	Ora vengono inseriti i dati su database. Viene inserito un dato per volta. Gli altri sono impostati di default a NULL.
	now() restituisce l'ora attuale.
	*/
    $query = "INSERT INTO $tabella VALUES ($temperatura, $umidita, $pressione, $luce, $battery, now())";
  	$result = mysqli_query($con, $query);
  } else {
  	echo('Impossibile connettersi al database.');
  }

?>