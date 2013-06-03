<?php

include_once "annotations.php";

function dao_open() {
    global $db, $operation_stmt, $code_ref_stmt, $io_stmt, $object_stmt, $io_for_object_stmt, $io_for_parent_io_stmt;

    $db = new SQlite3("test/test.sq3", SQLITE3_OPEN_READONLY);
    # Get operation for a given id
    $operation_stmt = $db->prepare("SELECT operation_id as id, name, parent_id, code_start_id, code_end_id, terminated_time FROM operation WHERE operation_id = :id");
    # get a reference to code by reference id
    $code_ref_stmt = $db->prepare("SELECT code_id as id, file, line FROM code WHERE code_id = :id");
    # Get list of IOs for given operation (only the top-level ones)
    $io_stmt = $db->prepare('SELECT io_id as id, io_time, object_id, operation_id, name, value, readwrite, note, code_id FROM io WHERE operation_id = :id AND parent_id = -1 ORDER BY io_time ASC');
    # Get object of given id
    $object_stmt = $db->prepare('SELECT object_id as id, type, deallocated_time FROM object where object_id = :id');
    # All parent IOs for object
    $io_for_object_stmt = $db->prepare('SELECT io_id as id, io_time, object_id, operation_id, name, value, readwrite, note, code_id FROM io where object_id = :id AND parent_id = -1 ORDER BY io_time ASC');
    # All IOs for parent io
    $io_for_parent_io_stmt = $db->prepare('SELECT io_id as id, io_time, object_id, operation_id, name, value, readwrite, note, code_id FROM io WHERE parent_id = :id ORDER BY io_id ASC');
}

function dao_close() {
    global $db;

    $db->close();
}

function get_code($id)
{
    global $code_ref_stmt;
    $code_ref_stmt->bindParam("id", $id, SQLITE3_INTEGER);
    return $code_ref_stmt->execute()->fetchArray();
}

function get_operation($id)
{
    global $operation_stmt;
    $annotations = get_annotations();

    $operation_stmt->bindParam('id', $id);
    $result = $operation_stmt->execute()->fetchArray();
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

# Direct children of operation of given id.
function get_operation_children($id)
{
    global $db;
    return $db->query("SELECT operation_id as id FROM operation WHERE parent_id = $id");
}

function get_object($id)
{
    global $object_stmt;
    $object_stmt->bindParam('id', $id);
    $result = $object_stmt->execute()->fetchArray();
    return $result;
}

function get_io_for_object($id)
{
    global $io_for_object_stmt;
    $io_for_object_stmt->bindParam('id', $id);
    $ios = $io_for_object_stmt->execute();
    $result = array();
    while ($io = $ios->fetchArray()) {
        $io['op_type'] = 'io';
        $result[$io['id']] = $io;
    }
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
    while ($io = $ios->fetchArray()) {
        $io['op_type'] = 'io';
        $result[$io['io_time']] = $io;
    }
    return $result;
}

# List child IOs for a given parent IO
# $id - io_id of the parent io
function get_child_io($id) {
    global $io_for_parent_io_stmt;
    $io_for_parent_io_stmt->db2_bind_param('id', $id);
    $ios = $io_for_parent_io_stmt->execute();
    $result = array();
    while ($io = $ios->fetchArray()) {
        $io['op_type'] = 'io';
        $result[] = $io;
    }
    return $result;
}

function get_op_children($id, &$result)
{
    $children = get_operation_children($id);
    while ($child = $children->fetchArray()) {
        $child_operation = get_operation($child['id']);
        $child_operation['op_type'] = 'op';
        $result[$child_operation['id']] = $child_operation;
    }
    return $result;
}
