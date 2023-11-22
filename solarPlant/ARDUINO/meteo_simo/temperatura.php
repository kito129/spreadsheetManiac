<html>
<head>
<title>Grafico temperatura</title>
<script src="Chart.js"></script>
</head>
<body>
<p>
  <?php 
  
  
// -----------------------------------------

//Le tue impostazioni database - TODO
$username="user";
$password="password";
$database="database";
$tabella= "meteo";
$host="host";

// -----------------------------------------



$conness = mysqli_connect($host, $username, $password);
mysqli_select_db($conness,$database) or die('Errore connessione...');


  $j = 1;
while ($j <= 48)
{
	$t[$j] = 0;
	$p[$j] = 0;
	$l[$j] = 0;
	$u[$j] = 0;
	$j = $j + 1;
}

  $i = 0;
  $q6 = "SELECT temperatura, pressione, luce, umidita, time_stamp FROM meteo ORDER BY time_stamp DESC";
  
  $r6 = mysqli_query($conness,$q6);
  $check_meteo = 0;
  $j = 48;
  $u = 1;
  $last = 0;
  while((list($temperatura, $pressione, $luce, $umidita, $time_stamp) = mysqli_fetch_row($r6)) && ($check_meteo == 0))
  {
  	if ($last == 0)
  	{
  		$last_date = strtotime($time_stamp);
		$next_date = date("H:i:s, d-m-Y", ($last_date + 3560));
		$last = 1;
	}
  	if ($temperatura != NULL)
  	{
  		if ($i == 0)
		{
			$h[$j] = substr($time_stamp, 11, 2); 
			$t[$j] = $temperatura;
			$i = 1;
			$last_gg = substr($time_stamp, 8, 2);
			$last_mm = substr($time_stamp, 5, 2);
			$last_aaaa = substr($time_stamp, 0, 4);
			$last_hh = substr($time_stamp, 11, 8);
		}
		else
		{
			$j = $j - 1;
			$h[$j] = substr($time_stamp, 11, 2); 
			if ($h[$j] == $h[$j+1])
			{
				$t[$j+1] = (($t[$j+1]*$u)+$temperatura)/($u+1);
				$j = $j + 1;
				$u = $u + 1;
			}
			else
			{
				$t[$j] = $temperatura;
				$u = 1;
			}
		}
		if ($j == 0)
		{
			$check_meteo = 1;
		}
		
	}
  }

$j = 48;
while ($j > 0)
{
	$t[$j] = substr($t[$j], 0, 4);
	$j = $j - 1;
}
  
  ?>
</p>

<h1 align="center">ARDUINO METEO - TEMPERATURA [°C]</h1>
<div align="left">[°C]        Temperature readings ±0.5°C accuracy</div>

<div style="width:80%">
			<div>
			  <canvas id="canvas" height="450" width="1200"></canvas>
			</div>
</div>
<div align="center"><br />[hh:mm]</div>

	<div>[ <u>Last detection</u>: <?php echo $last_hh; ?>, <?php echo $last_gg; ?>-<?php echo $last_mm; ?>-<?php echo $last_aaaa; ?> ---> <u>Update frequency</u>: approximately 1 hour ---> <u>Next detection</u>: <?php echo $next_date; ?> ]</div>
	
<p align="right">Graphs by <a href="http://www.chartjs.org/" target="_blank" class="link_title">Chartjs.org</a></p>
<script>
		var randomScalingFactor = function(){ return Math.round(Math.random()*100)}; //Se vuoi inserire dati a caso usa questa
		var lineChartData = {
			labels : [<?php 
			$y = 1;
			while($y <= 47)
			{
			echo '"'; echo $h[$y]; echo ':00",';
			$y = $y + 1;
			}
			echo '"'; echo $h[$y]; echo ':00"';
			?>],
			datasets : [
				{
					label: "Temperatura",
					fillColor : "rgba(25,182,110,0.2)",
					strokeColor : "rgba(25,182,110,1)",
					pointColor : "rgba(15,105,68,1)",
					pointStrokeColor : "#fff",
					pointHighlightFill : "#fff",
					pointHighlightStroke : "rgba(220,220,220,1)",
					data : [<?php 
			$y = 1;
			while($y <= 47)
			{
			echo $t[$y]; echo ', ';
			$y = $y + 1;
			}
			echo $t[$y];
			?>]
				}
			]

		}

	window.onload = function(){
		var ctx = document.getElementById("canvas").getContext("2d");
		window.myLine = new Chart(ctx).Line(lineChartData, {
			responsive: true
		});
	}


	  </script>

	  

</body>
</html>
