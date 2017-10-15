<?php 
$im = imagecreatefrompng("font.png");
$w = imagesx($im);
$h = imagesy($im);

$char_count=26+10+1;
$char_width=$w/$char_count;
$matrix=array();
for($char=0;$char<$char_count;$char++){
	$x0=$char_width*$char;
	$matrix[$char]=array();
	for($y=0;$y<$h;$y++){
		$rowval=0;
		for($x=$x0;$x<$x0+$char_width;$x++){
			$rowval*=2;
			$c = imagecolorat($im,$x,$y);
			if(imagecolorat($im,$x,$y)==0){
				$rowval+=1;
			}
		}
		$matrix[$char][$y]=$rowval;
	}
}

echo "{\n";

for($i=0;$i<count($matrix);$i++){
	$rows=$matrix[$i];
	if($i){echo ",";}
	echo "{";
	for($j=0;$j<count($rows);$j++){
		$row=$rows[$j];
		if($j){echo ",";}
		echo $row;
	}
	echo "}\n";
}
echo "}";
