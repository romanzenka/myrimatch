<?php
// Visualize values of specific types

function parse_vector($vector)
{
    $items = preg_split('/\\\\t/', $vector);
    $values = array();
    foreach ($items as $i) {
        $values[] = doubleval($i);
    }
    return $values;
}

function dtto()
{
    return "&#x22ee;";
}

function vis_value($value, $prev_value, &$allSame)
{
    $result = '';
    if ($value === $prev_value) {
        $result .= dtto();
    } else {
        $result .= '<pre>' . stripcslashes($value) . '</pre>';
        $allSame = false;
    }
    return $result;
}

function vis_enum($value, $prev_value, $values, &$allSame)
{
    return vis_value($values[$value], $prev_value !== null ? $values[$prev_value] : '', $allSame);
}

// A vector of numbers. If too few, display it as a list of numbers, otherwise plot
function vis_vector($vector, $id, $prev_vector = '', &$allSame)
{
    if ($vector === $prev_vector) {
        return dtto();
    }
    $result = '';

    $allSame = false;

    $width = 824;
    $height = 100;

    $values = parse_vector($vector);
    $prev_values = null;
    $prev = false;
    if ($prev_vector !== '' && $prev_vector != null) {
        $prev_values = parse_vector($prev_vector);
        if (count($prev_values) == count($values)) {
            $prev = true;
        }
    }
    $maxval = max(0, max($values));
    $minval = min(0, min($values));
    $valspan = $maxval - $minval;
    $zeroHeight = $height * $maxval / $valspan;
    $magnification = $height;

    $num = count($values);

    if ($num > 5) {
        $bar_width = $width / $num;
        ob_start();
        ?>
        <canvas id="canvas<?php echo $id ?>" width="<?php echo $width; ?>" height="<?php echo $height; ?>">

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
                        $h = $height * $y * $magnification / $valspan;
                        if($h < $height) {
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
    } else {
        foreach($values as $x => $y) {
            $h = '';
            if($prev) {
                $prev_value = $prev_values[$x];
                if($y!=$prev_value) {
                    $h .= ' class="changed"';
                }
            }
            $result .= "<pre$h>".htmlentities($y)."</pre>";
            $result .= "</span>";
        }
    }
    return $result;
}

// Parse a spectrum string (tab separated, comma separated mz/int pairs)
// into two arrays - mzs and ints. Return an array containing those two as sub-arrays.
function parse_spectrum($data)
{
    $items = preg_split('/\\\\t/', $data);
    $mzs = array();
    $ints = array();
    $classes = array();
    foreach ($items as $i) {
        $parts = preg_split('/\s*,\s*/', $i);
        $mzs[] = doubleval($parts[0]);
        $ints[] = doubleval($parts[1]);
        if (count($parts) > 2) {
            $classes[] = intval($parts[2]);
        } else {
            $classes[] = -1;
        }
    }
    $result = array();
    $result[] = $mzs;
    $result[] = $ints;
    $result[] = $classes;
    return $result;
}

# Data format: tab separated triplets 'm/z, intensity[, class]'
function vis_spectrum($data, $id, $prevData = '', &$allSame)
{
    if ($data === $prevData) {
        return dtto();
    }
    $allSame = false;

    $mzsInts = parse_spectrum($data);
    $mzs = $mzsInts[0];
    $ints = $mzsInts[1];
    $classes = $mzsInts[2];
    $minClass = min($classes);
    $maxClass = max($classes);
    $maxIntensity = max($ints);
    $minX = min($mzs);
    $maxX = max($mzs);

    $hasPrevData = false;
    $mzs2 = array();
    $ints2 = array();
    if ($prevData != '') {
        $hasPrevData = true;
        $mzsInts2 = parse_spectrum($prevData);
        $mzs2 = $mzsInts2[0];
        $ints2 = $mzsInts2[1];
        $minX = min($minX, min($mzs2));
        $maxX = max($maxX, max($mzs2));
    }
    $width = 824;
    $extra_bottom = false;
    if ($minClass < $maxClass) {
        $extra_bottom = true;
    }
    $spectrumHeight = 100;
    $magnification = $spectrumHeight;
    $height = $spectrumHeight + ($extra_bottom ? 30 : 0);
    $bar_width = 1.0;
    ob_start();
    ?>
    <canvas id="canvas<?php echo $id ?>" width="<?php echo $width; ?>" height="<?php echo $height; ?>">

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
            for($i = 0; $i<count($mzs); $i++) {
                $mz = $mzs[$i];
                $intensity = $ints[$i];
                $xpos = $width*($mz - $minX)/($maxX-$minX);
                if($maxIntensity>0) {
                    $h = $spectrumHeight * $magnification * $intensity / $maxIntensity;
                    if($h<$spectrumHeight) {
                        echo "ctx.fillRect($xpos, ".($spectrumHeight-$h).", $bar_width, $h);";
                    }
                }
            }

            if($extra_bottom) {
                for($i = 0; $i<count($mzs); $i++) {
                    $mz = $mzs[$i];
                    $class = $classes[$i];
                    $xpos = $width*($mz - $minX)/($maxX-$minX);
                    switch($class) {
                        case 1: $fill = '#ff4444'; break;
                        case 2: $fill = '#44bb44'; break;
                        case 3: $fill = '#4444ff'; break;
                        default:
                            $fill = '#ffffff';
                    }
                    echo "ctx.fillStyle='$fill';";
                    echo "ctx.fillRect($xpos, ".($spectrumHeight+1+($class-1)*10).", $bar_width, ".(10).");";
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
                        $h = min($spectrumHeight, $spectrumHeight * $magnification * $intensity / $maxIntensity);
                        echo "ctx.fillRect($xpos, ".($spectrumHeight-$h).", $bar_width, $h);";
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
                    $new = false;
                    if($hasPrevData) {
                        if(!in_array($mz, $mzs2)) {
                            $new = true;
                        }
                    }
                    if($new) {
                        echo "ctx.fillStyle=\"#00ff00\";";
                    } else {
                        echo "ctx.fillStyle=\"#000000\";";
                    }

                    $xpos = $width*($mz - $minX)/($maxX-$minX);
                    $h = $spectrumHeight * $intensity / $maxIntensity;
                    echo "ctx.fillRect($xpos, ".($spectrumHeight-$h).", $bar_width, $h);";
                }
            }
        ?>
    </script>
    <?php

    $result = ob_get_contents();
    ob_end_clean();
    return $result;
}

function vis_map($id, &$allSame)
{
    $ios = get_child_ios($id);

    $childrenSame = true;
    $result2 = '';
    $result2 .= '<table>';
    foreach ($ios as $cio) {
        if (endsWith($cio['parent_relation'], 'key')) {
            $result2 .= '<tr>';
        }
        $result2 .= '<td>';
        $object = get_object($cio['object_id']);
        $result2 .= get_object_link($object, '&gt;', $cio['operation_id']);
        $result2 .= '</td><td>';
        $result2 .= render_value($cio, $object, $childrenSame);
        $result2 .= '</td>';
        if (endsWith($cio['parent_relation'], 'value')) {
            $result2 .= '</tr>';
        }
    }
    $result2 .= '</table>';
    if ($childrenSame) {
        $result = dtto();
    } else {
        $allSame = false;
        $result = $result2;
    }
    return $result;
}

function vis_structure($io, $prev_value, &$allSame)
{
    $id = $io['id'];
    $value = $io['value'];

    $ios = get_child_ios($id);
    $result = '';
    if (count($ios) == 0) {
        $result = vis_value($value, $prev_value, $allSame);
    } else {
        $childrenSame = true;
        $result2 = '';
        $result2 .= '<table>';
        $dttoMode = false;
        $dttoNames = '';
        foreach ($ios as $cio) {
            $childSame = true;
            $object = get_object($cio['object_id']);
            $result3 = '<tr><td>';
            $result3 .= get_object_link($object, $cio['parent_relation'], $cio['operation_id']);
            $result3 .= '</td><td>';
            $result3 .= render_value($cio, $object, $childSame);
            $result3 .= '</td></tr>';
            $childrenSame = $childrenSame && $childSame;
            if (!$childSame) {
                if ($dttoMode) {
                    $result2 .= "<tr><td title=\"These members are unchanged: $dttoNames\">&#x22ee;</td><td></td></tr>";
                    $dttoMode = false;
                    $dttoNames = '';
                }
                $result2 .= $result3;
            } else {
                $dttoMode = true;
                $dttoNames .= " " . htmlentities($cio['parent_relation']);
            }
        }
        $result2 .= '</table>';
        if ($childrenSame) {
            $result = dtto();
        } else {
            $allSame = false;
            $result = $result2;
        }
    }
    return $result;
}