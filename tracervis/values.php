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
    $values = parse_vector($vector);
    $prev_values = null;
    $prev = false;
    if($prev_vector != '') {
        $prev_values = parse_vector($prev_vector);
        if(count($prev_values)==count($values)) {
            $prev = true;
        }
    }
    $maxval = max($values);
    $num = count($values);
    $width = 500;
    $height = 50;
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
        ctx.fillStyle = '#FF0000';
<?php
    if($maxval > 0) {
        foreach($values as $x => $y) {
            $xpos = $width * $x / $num;
            $h = $height * $y / $maxval;
            if($prev) {
                $prev_value = $prev_values[$x];
                if($y==$prev_value) {
                    echo 'ctx.fillStyle = \'#000000\';';
                } else {
                    echo 'ctx.fillStyle = \'#FF0000\';';
                }
            }
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

function vis_spectrum($data, $id) {
    $mzsInts = parse_spectrum($data);
    $mzs = $mzsInts[0];
    $ints = $mzsInts[1];
    $maxIntensity = max($ints);
    $minX = min($mzs);
    $maxX = max($mzs);
    $width = 600;
    $height = 80;
    $bar_width = 0.5;
    ob_start();
?>
    <canvas id="canvas<?php echo $id?>" width="<?php echo $width; ?>" height="<?php echo $height; ?>">

    </canvas>
    <script type="text/javascript">
        var canvas = document.getElementById('canvas<?php echo $id?>');
        var ctx = canvas.getContext('2d');
        ctx.fillStyle = "#eeeeee";
        ctx.fillRect(0, 0, <?php echo $width; ?>, <?php echo $height; ?>);
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
    ctx.fillStyle = '#000000';
<?php
    if($maxIntensity > 0) {
        for($i = 0; $i<count($mzs); $i++) {
            $mz = $mzs[$i];
            $intensity = $ints[$i];
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