<?php

echo "Allumage de la ventilation de l'aquarium 1 <br />";
shell_exec("sudo python /home/pi/code/fanOn.py");

$wt1=shell_exec ("sudo python /home/pi/code/getWT1.py");
echo "<br />temperature de l'eau : ".$wt1."°C<br />";

echo '<li><a href="fansOff.php">Eteindre la ventilation de l\'aquarium 1</a></li>';
echo '<a href="index.php">retour</a><br />';
?>