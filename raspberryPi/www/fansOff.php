<?php

echo "extinction de la ventilation de l'aquarium 1.<br />";
shell_exec("sudo python /home/pi/code/fanOff.py");

$wt1=shell_exec ("sudo python /home/pi/code/getWT1.py");
echo "<br />temperature de l'eau : ".$wt1."°C<br />";

echo '<a href="fansOn.php">Allumage de la ventilation de l\'aquarium 1</a><br />';

echo '<a href="index.php">retour</a><br />';



?>