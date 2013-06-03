<?php
// Visualize values of specific types

function parse_vector($vector) {
    $items = preg_split('/\\\\t/', $vector);
    $values = array();
    foreach($items as $i) {
        $values[] = doubleval($i);
    }
    return $values;
}

// A vector of numbers
function vis_vector($vector, $id, $prev_vector='') {
    $width = 1324;
    $height = 80;

    $values = parse_vector($vector);
    $prev_values = null;
    $prev = false;
    if($prev_vector != '') {
        $prev_values = parse_vector($prev_vector);
        if(count($prev_values)==count($values)) {
            $prev = true;
        }
    }
    $maxval = max(0, max($values));
    $minval = min(0, min($values));
    $valspan = $maxval - $minval;
    $zeroHeight = $height * $maxval / $valspan;

    $num = count($values);
    $bar_width = $width/$num;
    ob_start();
?>
    <canvas id="canvas<?php echo $id?>" width="<?php echo $width; ?>" height="<?php echo $height; ?>">

    </canvas>
    <script type="text/javascript">
        var canvas = document.getElementById('canvas<?php echo $id?>');
        var ctx = canvas.getContext('2d');
        ctx.fillStyle = "#eeeeee";
        ctx.fillRect(0, 0, <?php echo $width; ?>, <?php echo $height; ?>);

        ctx.fillStyle = '#dddddd';

<?php
    if($maxval > 0) {
        foreach($values as $x => $y) {
            $xpos = $width * $x / $num;
            $h = $height * $y * 100.0 / $valspan;
            if($prev) {
                $prev_value = $prev_values[$x];
                if($y==$prev_value) {
                    echo 'ctx.fillStyle = \'#dddddd\';';
                } else {
                    echo 'ctx.fillStyle = \'#ddddee\';';
                }
            }
            echo "ctx.fillRect($xpos, ".($zeroHeight-$h).", $bar_width, $h);";
        }
    }
?>

        ctx.fillStyle = '#000000';

<?php
    if($maxval > 0) {
        foreach($values as $x => $y) {
            $xpos = $width * $x / $num;
            $h = $height * $y / $valspan;
            if($prev) {
                $prev_value = $prev_values[$x];
                if($y==$prev_value) {
                    echo 'ctx.fillStyle = \'#000000\';';
                } else {
                    echo 'ctx.fillStyle = \'#0000dd\';';
                }
            }
            echo "ctx.fillRect($xpos, ".($zeroHeight-$h).", $bar_width, $h);";
        }
    }
?>
        ctx.fillStyle = "#888888";
        ctx.textAlign = "left";
        ctx.textBaseline = "top";
        ctx.font = "normal 10px Arial";
        ctx.fillText("< <?php echo 0 ?>", 0, 11);
        ctx.textAlign = "center";
        ctx.fillStyle = "#888888";
        ctx.fillText("^ <?php echo $maxval ?>", <?php echo $width/2 ?>, 0);
        ctx.textAlign = "right";
        ctx.fillText("<?php echo count($values) ?> >", <?php echo $width ?>, 11);
        ctx.textAlign = "center";
        ctx.textBaseline = "bottom";
        ctx.fillText("<?php echo $minval ?> v", <?php echo $width/2 ?>, <?php echo $height ?>);


    </script>
<?php

    $result = ob_get_contents();
    ob_end_clean();
    return $result;
}

// Parse a spectrum string (tab separated, comma separated mz/int pairs)
// into two arrays - mzs and ints. Return an array containing those two as sub-arrays.
function parse_spectrum($data) {
    $items = preg_split('/\\\\t/', $data);
    $mzs = array();
    $ints = array();
    foreach($items as $i) {
        $parts = preg_split('/\s*,\s*/', $i);
        $mzs[] = doubleval($parts[0]);
        $ints[] = doubleval($parts[1]);
    }
    $result = array();
    $result[] = $mzs;
    $result[] = $ints;
    return $result;
}

function vis_spectrum($data, $id, $prevData='') {
    $mzsInts = parse_spectrum($data);
    $mzs = $mzsInts[0];
    $ints = $mzsInts[1];
    $maxIntensity = max($ints);
    $minX = min($mzs);
    $maxX = max($mzs);

    $hasPrevData = false;
    $mzs2 = array();
    $ints2 = array();
    if($prevData!='') {
        $hasPrevData = true;
        $mzsInts2 = parse_spectrum($prevData);
        $mzs2 = $mzsInts2[0];
        $ints2 = $mzsInts2[1];
        $minX = min($minX, min($mzs2));
        $maxX = max($maxX, max($mzs2));
    }
    $width = 1324;
    $height = 80;
    $bar_width = 1.0;
    ob_start();
?>
    <canvas id="canvas<?php echo $id?>" width="<?php echo $width; ?>" height="<?php echo $height; ?>">

    </canvas>
    <script type="text/javascript">
        var canvas = document.getElementById('canvas<?php echo $id?>');
        var ctx = canvas.getContext('2d');
        ctx.fillStyle = "#eeeeee";
        ctx.fillRect(0, 0, <?php echo $width; ?>, <?php echo $height; ?>);
        ctx.fillStyle = "#888888";
        ctx.textAlign = "left";
        ctx.textBaseline = "top";
        ctx.font = "normal 10px Arial";
        ctx.fillText("< <?php echo $minX ?>", 0, 11);
        ctx.textAlign = "center";
        ctx.fillStyle = "#888888";
        ctx.fillText("^ <?php echo $maxIntensity ?>", <?php echo $width/2 ?>, 0);
        ctx.textAlign = "right";
        ctx.fillText("<?php echo $maxX ?> >", <?php echo $width ?>, 11);
        ctx.fillText("<?php echo count($ints) ?> peaks", <?php echo $width ?>, 0);
        ctx.fillStyle = '#cccccc';
<?php
    if($maxIntensity > 0) {
        for($i = 0; $i<count($mzs); $i++) {
            $mz = $mzs[$i];
            $intensity = $ints[$i];
            $xpos = $width*($mz - $minX)/($maxX-$minX);
            $h = min($height, $height * 100.0 * $intensity / $maxIntensity);
            echo "ctx.fillRect($xpos, ".($height-$h).", $bar_width, $h);";
        }
    }
?>
        ctx.fillStyle = "#ff5555";
<?php
    if($hasPrevData > 0) {
        $maxDeleted = 0;
        $countDeleted = 0;
        for($i = 0; $i<count($mzs2); $i++) {
            $mz = $mzs2[$i];
            if(!in_array($mz, $mzs)) {
                $intensity = $ints2[$i];
                $xpos = $width*($mz - $minX)/($maxX-$minX);
                $h = min($height, $height * 100 * $intensity / $maxIntensity);
                echo "ctx.fillRect($xpos, ".($height-$h).", $bar_width, $h);";
                $countDeleted++;
                if($intensity>$maxDeleted) {
                    $maxDeleted = $intensity;
                }
            }
        }
        if($maxDeleted > 0) {
?>
            ctx.textAlign = "center";
            ctx.fillStyle = "#ff5555";
            ctx.fillText("<?php echo $countDeleted?> deleted ^ <?php echo $maxDeleted ?>", <?php echo $width/2 ?>, 11);
<?php
        }
    }
?>
    ctx.fillStyle = '#000000';
<?php
    if($maxIntensity > 0) {
        for($i = 0; $i<count($mzs); $i++) {
            $mz = $mzs[$i];
            $intensity = $ints[$i];
            if($hasPrevData) {
                if(in_array($mz, $mzs2)) {
                    echo "ctx.fillStyle=\"#000000\";";
                } else {
                    echo "ctx.fillStyle=\"#00ff00\";";
                }
            }

            $xpos = $width*($mz - $minX)/($maxX-$minX);
            $h = $height * $intensity / $maxIntensity;
            echo "ctx.fillRect($xpos, ".($height-$h).", $bar_width, $h);";
        }
    }
?>
    </script>
<?php

    $result = ob_get_contents();
    ob_end_clean();
    return $result;
}