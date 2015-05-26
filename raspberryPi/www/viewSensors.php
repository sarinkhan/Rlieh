<?php

echo "<h1>Visualisation des capteurs</h1>";
$wt1=shell_exec ("sudo python /home/pi/code/getWT1.py");
$wt2=shell_exec ("sudo python /home/pi/code/getWT2.py");
$at1=shell_exec ("sudo python /home/pi/code/getAT1.py");


echo "<br />temperature aquarium 1  (crevettes): ".$wt1."°C<br />";
echo "temperature aquarium 2 (guppys/khulis): ".$wt2."°C<br />";
echo "temperature air : ".$at1."°C<br />";


echo '<br /><a href="viewSensors.php">rafraichir</a><br />';
echo '<a href="index.php">retour</a><br />';
?>