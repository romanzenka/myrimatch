<?php

include_once "annotations.php";

class ReadWrite {
    const Read = 1;
    const Write = 2;
}

function dao_open() {
    global $db, $all_ops_stmt, $operation_stmt, $operation_id_stmt, $code_ref_stmt, $io_stmt, $object_stmt, $io_for_object_stmt, $io_for_parent_io_stmt, $operation_by_name_stmt, $io_for_object_parent_name_stmt;

    $db = new PDO("sqlite:test.sq3");
    # Get all ops
    $all_ops_stmt = $db->prepare("SELECT operation_id as id, name, parent_id, code_start_id, code_end_id, terminated_time FROM operation ORDER BY operation_id");
    # Get operation for a given id
    $operation_stmt = $db->prepare("SELECT operation_id as id, name, parent_id, code_start_id, code_end_id, terminated_time FROM operation WHERE operation_id = :id");
    # Operation id for parent operation
    $operation_id_stmt = $db->prepare('SELECT operation_id as id FROM operation WHERE parent_id = :id');
    # get a reference to code by reference id
    $code_ref_stmt = $db->prepare("SELECT code_id as id, file, line FROM code WHERE code_id = :id");
    # Get list of IOs for given operation (only the top-level ones)
    $io_stmt = $db->prepare('SELECT io_id as id, io_time, object_id, operation_id, name, value, readwrite, note, code_id FROM io WHERE operation_id = :id AND parent_id = -1 ORDER BY io_time ASC');
    # Get object of given id
    $object_stmt = $db->prepare('SELECT object_id as id, type, deallocated_time FROM object where object_id = :id');
    # All parent IOs for object
    $io_for_object_stmt = $db->prepare('SELECT io_id as id, io_time, object_id, operation_id, name, value, readwrite, note, code_id FROM io where object_id = :id ORDER BY io_time ASC');
    # All IOs for parent io
    $io_for_parent_io_stmt = $db->prepare('SELECT io_id as id, io_time, object_id, operation_id, name, value, readwrite, note, code_id, parent_id FROM io WHERE parent_id = :id ORDER BY io_id ASC');
    # Operation of a given name for parent operation
    $operation_by_name_stmt = $db->prepare('SELECT operation_id as id, name, parent_id, code_start_id, code_end_id, terminated_time FROM operation WHERE name = :name AND parent_id = :parent ORDER BY operation_id');
    # All IOs for given object, parent IO and name
    $io_for_object_parent_name_stmt = $db->prepare('SELECT io_id as id, io_time, object_id, operation_id, name, value, readwrite, note, code_id FROM io where operation_id = :operation_id AND parent_id = :parent_io AND name = :name AND note=:note ORDER BY io_time ASC');
    check_error();
}

function dao_close() {
    global $db;

    $db->close();
}

function get_code($id)
{
    global $code_ref_stmt;
    $code_ref_stmt->bindParam("id", $id, PDO::PARAM_INT);
    $code_ref_stmt->execute();
    $result = $code_ref_stmt->fetch(PDO::FETCH_ASSOC);
    $code_ref_stmt->closeCursor();
    return $result;
}

function annotate($result) {
    $annotations = get_annotations();
    if (array_key_exists($result['name'], $annotations)) {
        $annotation = $annotations[$result['name']];
        $result['sdesc'] = $annotation['sdesc'];
        $result['desc'] = $annotation['desc'];
    } else {
        $result['sdesc'] = $result['name'];
        $result['desc'] = '';
    }
    return $result;
}

function get_operation($id)
{
    global $operation_stmt;

    $operation_stmt->bindParam('id', $id);
    $operation_stmt->execute();
    $result = $operation_stmt->fetch(PDO::FETCH_ASSOC);
    $operation_stmt->closeCursor();
    return annotate($result);
}

function check_error() {
    global $db;
    if($db->errorCode()!=0) {
        $errorInfo = $db->errorInfo();
        echo $errorInfo[2];
    }
}

# Return a list of operations of a given name for given parent
# Example syntax:
# op1(*)/op2(0)/op3(1)
# ... will enumerate all operations named op1, then take 1-st sub-operation named op2 of each, then 2-nd sub-operation named op3 of that
# .. all results are returned
function get_operations_by_name($path, $parent_id=-1) {
    global $operation_by_name_stmt;
    $names = preg_split('/\//', $path, 2);
    $current = $names[0];
    $remaining = $names[1];
    preg_match('/^(?P<name>.*)\((?P<count>.*)\)$/', $current, $matches);
    $name = $matches['name'];
    $count = $matches['count'];

    $operation_by_name_stmt->bindParam('parent', $parent_id);
    $operation_by_name_stmt->bindParam('name', $name);
    $operation_by_name_stmt->execute();
    $ops_array = array();
    while($op = $operation_by_name_stmt->fetch(PDO::FETCH_ASSOC)) {
        $ops_array[] = $op;
    }
    $operation_by_name_stmt->closeCursor();
    $result = array();
    $i = 0;
    foreach($ops_array as $op) {
        if($count=='*' || intval($count)==$i) {
            if($remaining=='') {
                $result[] = annotate($op);
            } else {
                $more_ops = get_operations_by_name($remaining, intval($op['id']));
                $result = array_merge($result, $more_ops);
            }
        }
        $i++;
    }
    return $result;
}

# Return list of ios matching a particular name within a given operation
# The ios are separated by a / - e.g. 'config/digestionConfig/minimumSpecificity' for the structures
function get_io_by_name($operation_id, $path, $note='', $parent_io=-1) {
    global $io_for_object_parent_name_stmt;
    $names = preg_split('/\//', $path, 2);
    $name = $names[0];
    $remaining = $names[1];

    $io_for_object_parent_name_stmt->bindParam('operation_id', $operation_id);
    $io_for_object_parent_name_stmt->bindParam('name', $name);
    $io_for_object_parent_name_stmt->bindParam('note', $note);
    $io_for_object_parent_name_stmt->bindParam('parent_io', $parent_io);
    // echo "SELECT * from io where operation_id = $operation_id AND name = \"$name\" and note = \"$note\" and parent_id = $parent_io;";
    $io_for_object_parent_name_stmt->execute();
    $ops = array();
    while($op = $io_for_object_parent_name_stmt->fetch(PDO::FETCH_ASSOC)) {
        $ops[] = $op;
    }
    $io_for_object_parent_name_stmt->closeCursor();

    $result = array();
    foreach($ops as $op) {
        if($remaining=='') {
            $result[] = $op;
        } else {
            $result = array_merge($result, get_io_by_name($operation_id, $remaining, $note, $op['id']));
        }
    }
    return $result;
}

function get_object($id)
{
    global $object_stmt;
    $object_stmt->bindParam('id', $id);
    $object_stmt->execute();
    $result = $object_stmt->fetch(PDO::FETCH_ASSOC);
    $object_stmt->closeCursor();
    return $result;
}

function get_io_for_object($id)
{
    global $io_for_object_stmt;
    $io_for_object_stmt->bindParam('id', $id);
    $io_for_object_stmt->execute();
    $result = array();
    while ($io = $io_for_object_stmt->fetch(PDO::FETCH_ASSOC)) {
        $io['op_type'] = 'io';
        $result[$io['id']] = $io;
    }
    $io_for_object_stmt->closeCursor();
    return $result;
}

# An object can be referred to using many names
# pick the one that is most commonly used.
function get_name_for_object($object_ios) {
    $names = array();
    foreach($object_ios as $value) {
        $names[$value['note']]++;
    }
    natsort($names);
    end($names);
    if(key($names) == '*this' && count($names)>1) {
        prev($names);
        return key($names);
    }
    return key($names);
}

# List of IOs for a given operation
function get_op_ios($id, &$result)
{
    global $io_stmt;
    $io_stmt->bindParam('id', $id);
    $ios = $io_stmt->execute();
    while ($io = $io_stmt->fetch(PDO::FETCH_ASSOC)) {
        $io['op_type'] = 'io';
        $result[$io['io_time']] = $io;
    }
    $io_stmt->closeCursor();
    return $result;
}

# List child IOs for a given parent IO
# $id - io_id of the parent io
function get_child_ios($id) {
    global $io_for_parent_io_stmt;
    $io_for_parent_io_stmt->bindParam('id', $id);
    $ios = $io_for_parent_io_stmt->execute();
    $result = array();
    while ($io = $io_for_parent_io_stmt->fetch(PDO::FETCH_ASSOC)) {
        $io['op_type'] = 'io';
        $result[] = $io;
    }
    $io_for_parent_io_stmt->closeCursor();
    return $result;
}

function get_op_children($id, &$result)
{
    global $operation_id_stmt;
    $operation_id_stmt->bindParam('id', $id);
    $operation_id_stmt->execute();

    while ($child = $operation_id_stmt->fetch(PDO::FETCH_ASSOC)) {
        $child_operation = get_operation($child['id']);
        $child_operation['op_type'] = 'op';
        $result[$child_operation['id']] = $child_operation;
    }
    $operation_id_stmt->closeCursor();
    return $result;
}

function get_all_ops() {
    global $all_ops_stmt;
    $all_ops_stmt->execute();
    $result = array();
    $depth = array();
    $depth[-1] = 0;
    while($op = $all_ops_stmt->fetch(PDO::FETCH_ASSOC)) {
        $op['depth'] = $depth[$op['parent_id']]+1;
        $depth[$op['id']] = $op['depth'];
        $result[] = $op;
    }
    $all_ops_stmt->closeCursor();
    return $result;
}