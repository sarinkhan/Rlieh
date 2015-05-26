<?php

echo "<h1>Visualisation des capteurs</h1>";
$temp=shell_exec ("sudo python /home/pi/code/getTemp.py");

//echo $temp;

function strstr_after($haystack, $needle, $case_insensitive = false) {
    $strpos = ($case_insensitive) ? 'stripos' : 'strpos';
    $pos = $strpos($haystack, $needle);
    if (is_int($pos)) {
        return substr($haystack, $pos + strlen($needle));
    }
    // Most likely false or null
    return $pos;
}

function getSimpleXMLVal1($inputString, $xmlTag)
{
	$inTag='<'.$xmlTag.'>';
	$outTag='</'.$xmlTag.'>';
	$t1 = strstr_after($inputString, $inTag);
	return strstr($t1, $outTag, true);
}


$tempC= getSimpleXMLVal1($temp, 'tempC');
$tempF= getSimpleXMLVal1($temp, 'tempF');
//$t1 = strstr($temp, '<tempC>');
//$t1b=strstr($t1, '</tempC>', true);

echo "<br />temperature : ".$tempC."°C<br />";
echo "temperature : ".$tempF."°F<br />";

echo '<br /><a href="viewSensors.php">rafraichir</a><br />';
echo '<a href="index.php">retour</a><br />';
?>