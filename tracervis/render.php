<?php

include_once "values.php";

# Code for rendering objects into HTML

# An array from objId to previous io for an object.
$prev_ios = array();

#### Code links ####

# Link to a line in code
function get_code_url($id, $code)
{
    $res = get_code($id);
    $file = urlencode($res['file']);
    $line = urlencode($res['line']);
    $code = urlencode($code);
    return 'source.php?file=' . "$file&line=$line&code=$code#$line";
}

# Link to a code block (has start+end, first line is highlighted)
function get_code_block_link($start_id, $end_id)
{
    $res = get_code($start_id);
    $file = $res['file'];
    $line = $res['line'];

    $res = get_code($end_id);
    $file_end = $res['file'];
    $line_end = $res['line'];

    if ($file_end <> $file) {
        return "<div class=\"error\">Operation ends in a different file</div>";
    }

    return 'source.php' . "?file=$file&line=$line&first=$line&last=$line_end#$line";
}

#### Operations ####

function get_operation_link($operation, $object_id)
{
    $sdesc = $operation['sdesc'];
    $id = $operation['id'];
    return "<a href=\"dump.php?id=$id&object_id=$object_id\">$sdesc</a>";
}

# Go all the way to the root operation, produce html code for breadcrumbs
function get_operation_breadcrumbs($id)
{
    $result = "";
    while ($id != -1) {
        $operation = get_operation($id);
        $result = get_operation_link($operation, -1) . '<i class="icon-chevron-right"></i>' . $result;
        $id = $operation['parent_id'];
    }
    return $result;
}

function render_code_link($code_id, $code='') {
    return '<a href="' . get_code_url($code_id, $code) . '" title="'.$code.'"><i class="icon-tasks"></i></a>';
}

function startsWith($haystack, $needle)
{
    return !strncmp($haystack, $needle, strlen($needle));
}

function endsWith($haystack, $needle)
{
    $length = strlen($needle);
    if ($length == 0) {
        return true;
    }

    return (substr($haystack, -$length) === $needle);
}

# Render a value
function render_value($io, $io_obj, &$allSame) {
    global $prev_ios;

    $id = $io['id'];
    $value = $io['value'];

    $type = $io_obj['type'];
    $prev_value = null;
    if (array_key_exists($io_obj['id'], $prev_ios)) {
        $prev_value = $prev_ios[$io_obj['id']];
    }
    $result = '';
    if($type == 'std::vector<float>' || $type == 'std::vector<int>' || $type == 'std::vector<double>') {
        $result .= vis_vector($value, $id, $prev_value, $allSame);
    } else if($type == 'freicore::PeakPreData') {
        $result .= vis_spectrum($value, $id, $prev_value, $allSame);
    } else if($type == 'freicore::PeakSpectrum<PeakInfo>') {
        $result .= vis_spectrum($value, $id, $prev_value, $allSame);
    } else if($type == 'FragmentTypes') {
        $fragments = 'abcxyzZ';
        $result .= '<pre>' . substr($fragments, intval($value), 1) . '</pre>';
    } else if($type == 'freicore::myrimatch::MzToleranceRule') {
        $result .= vis_enum($value, $prev_value, array('0'=>'auto', '1'=>'monoisotopic', '2'=>'average'), $allSame);
    } else if($type == 'char') {
        if($value == $prev_value) {
            $result .= dtto();
        } else {
            $allSame = false;
            $result .= "<pre>$value</pre> ".chr(intval($value));
        }
    } else if(startsWith($type, 'std::map<')) {
        $result .= vis_map($id, $allSame);
    } else {
        $result .= vis_structure($io, $prev_value, $allSame);
    }
    $prev_ios[$io_obj['id']] = $value;
    return $result;
}

# The IO link can either reference the object it is input/outputting
# .. or the operation
# $r - loaded IO object
# $reference - link to the object details when 'object', link to the operation details when 'operation'
# $variable_name - override for the variable name, when '', no override
function render_io($r, $reference = 'object', $variable_name='') {
    $result = '';
    # Arrow for read/write/view
    switch ($r['readwrite']) {
        case 1:
            $result .= '<td><i title="read" class="icon-arrow-left"></i></td> ';
            break;
        case 2:
            $result .=  '<td><i title="write" class="icon-arrow-right"></i></td> ';
            break;
        case 3:
            $result .=  '<td><i title="value" class="icon-eye-open"></i></td> ';
            break;
    }
    // If we want to link to object, use the note for the operation to link to it
    $object = get_object($r['object_id']);
    $result .= '<td>';
    if($reference == 'object') {
        $result .=  get_object_link($object, $r['note'], $r['operation_id']);
        $result .= '</td><td>';
    } else {
        $result .= $r['note'];
        $result .= '</td><td>';
        $result .=  get_operation_link(get_operation($r['operation_id']), $r['object_id']);
    }
    $result .= '</td><td>';
    $allSame = true;
    $result .= render_value($r, $object, $allSame);
    $result .= '</td><td>';
    $result .= render_code_link($r['code_id'], $r['name']);
    $result .=  '</td>';
    return $result;
}

# Full render of an operation
function render_operation($id, $highlight_object_id)
{
    $operation = get_operation($id);
    $sdesc = htmlspecialchars($operation['sdesc']);
    $desc = $operation['desc'];
    $parent_id = $operation['parent_id'];
    $code_url = get_code_block_link($operation['code_start_id'], $operation['code_end_id']);
    $breadcrumbs = get_operation_breadcrumbs($parent_id);

    echo "$breadcrumbs";
    echo "<table cellpadding=\"2px\"><tr><td>";
    echo "<a href=\"dump.php?id=$parent_id\" class=\"btn btn-small\"><i class=\"icon-arrow-up\"></i></a></td>";
    echo "<td><a href=\"$code_url\" class=\"btn btn-small\"><i class=\"icon-tasks\"></i></a></td>";
    echo "<td><h2 style=\"margin-left: 0.3em;\">$sdesc</h2></td></tr></table>";
    echo "<p>$desc</p>";

    $result = array();
    get_op_ios($id, $result);
    get_op_children($id, $result);
    ksort($result);
    echo "<table class=\"table table-condensed\">";
    foreach ($result as $r) {
        $objId = $r['object_id'];
        if($r['op_type']=='io' && $objId==$highlight_object_id) {
            echo '<tr class="highlight">';
        } else {
            echo "<tr>";
        }
        echo '<td>'.$r['id'].'</td>';
        if ($r['op_type'] == 'op') {
            echo '<td><i class="icon-chevron-right"></i></td> ' .
                '<td colspan="4">' . get_operation_link($r, -1) . '</td>';
        } else if ($r['op_type'] == 'io') {
            echo render_io($r, 'object', '');
        }
        echo "</tr>";
    }
    echo "</table>";
}

function render_object($object_id, $operation_id) {
    $object = get_object($object_id);
    $ios = get_io_for_object($object_id);
    $name = get_name_for_object($ios);

    echo '<h2><span style="color: #ddd">' . htmlspecialchars($object['type']) . '</span> ' . htmlspecialchars($name) . '</h2>';

    echo '<table class="table table-condensed">';
    $prev_io = null;
    foreach($ios as $io) {
        if($io['operation_id'] == $operation_id) {
            echo "<tr class=\"highlight\">";
        } else {
            echo "<tr>";
        }
        echo '<td>'.$io['id'].'</td>';
        echo render_io($io, 'operation', $name, $prev_io);
        echo "</tr>";
        $prev_io = $io;
    }
    echo '</table>';
}

#### Objects ####

function get_object_link($object, $name, $operation_id=-1)
{
    $t = $object['type'];
    return '<a href="object.php?id=' . $object['id'] . '&operation_id='.$operation_id.'">' . $name . '</a>';
}
