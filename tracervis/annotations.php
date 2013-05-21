<?php

$annotations_enabled = false;

function get_annotations()
{
    global $annotations;
    if (defined($annotations)) {
        return $annotations;
    }
    $annotations = load_annotations();
    return $annotations;
}

# Load pretty names and descriptions from a file
function load_annotations()
{
    $state = 0;
    $id = '';
    $sdesc = '';
    $desc = '';
    $result = array();
    global $annotations_enabled;
    if ($annotations_enabled) {
        foreach (file('annotation.txt') as $line) {
            $line = rtrim($line);
            switch ($state) {
                case 0:
                    $id = $line;
                    $state = 1;
                    break;
                case 1:
                    $sdesc = $line;
                    $desc = '';
                    $state = 2;
                    break;
                case 2:
                    if ($line == '') {
                        $state = 0;
                        $result[$id] = array('sdesc' => $sdesc, 'desc' => $desc);
                    } else {
                        $desc .= $line . "<br/>\n";
                    }
                    break;
            }
        }
    }
    return $result;
}
