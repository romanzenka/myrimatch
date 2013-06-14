<?php
include_once "dao.php";
include_once "render.php";
include_once "home.php";

function summary_input_parameters() {
    echo "<h4>Command-line arguments</h4>";
    $op = get_operations_by_name("myrimatch::InitProcess(0)", 0);
    $obj = $op[0];
    $io = get_io_by_name($obj['id'], "args", "Arguments");
    echo render_value($io[0]);

    clear_previous();
    echo "<h4>Parsed configuration</h4>";
    $op = get_operations_by_name("all(0)/myrimatch::InitProcess(0)/Override arguments using a list of variables(0)");
    $operation = $op[0];
    $io = get_io_by_name($operation['id'], "*g_rtConfig", "Set variables to config");
    echo render_value($io[0]);
}

function echo_spectrum($operation, $path, $note, $hint='') {
    $io = get_io_by_name($operation['id'], $path, $note);
    $allSame = true;
    echo render_value($io[0], null, $allSame, $hint);
    echo "<br/>";
}

function summary_spectra() {
    $preprocesses = get_operations_by_name("all(0)/Spectrum::Preprocess(*)");
    foreach($preprocesses as $operation) {
        $io = get_io_by_name($operation['id'], "*this/nativeID", "*this start");
        echo "<h3>Spectrum ".$io[0]['value']."</h3>";
        echo "<table>";
        echo "<tr><td><a href=\"dump.php?id=".$operation['id']."\">Original</a></td><td>";
        echo_spectrum($operation, "*this/peakPreData", "*this start");
        echo "</td></tr>";

        $filter_op = get_operations_by_name("Filter out peaks above the largest precursor hypothesis mass(0)", $operation['id']);
        echo "<tr><td><a href=\"dump.php?id=".$filter_op[0]['id']."\">Remove peaks above precursor</a></td><td>";
        echo_spectrum($filter_op[0], "peakPreData", "Preprocessed peak data without peaks over max precursor mass");
        echo "</td></tr>";

        $filter_op = get_operations_by_name("PeakSpectrum::FilterByTIC(0)", $operation['id']);
        echo "<tr><td><a href=\"dump.php?id=".$filter_op[0]['id']."\">Filter by TIC</a></td><td>";
        echo_spectrum($filter_op[0], "peakPreData", "TIC-filtered list of preprocessed peaks");
        echo "</td></tr>";

        $filter_op = get_operations_by_name("PeakSpectrum::FilterByPeakCount(0)", $operation['id']);
        echo "<tr><td><a href=\"dump.php?id=".$filter_op[0]['id']."\">Filter by peak count</a></td><td>";
        echo_spectrum($filter_op[0], "peakPreData", "Count-filtered list of preprocessed peaks");
        echo "</td></tr>";

        $filter_op = get_operations_by_name("Remove water loss peaks(0)", $operation['id']);
        echo "<tr><td><a href=\"dump.php?id=".$filter_op[0]['id']."\">Remove water loss peaks</a></td><td>";
        echo_spectrum($filter_op[0], "peakPreData", "After removing water loss peaks");
        echo "</td></tr>";

        $filter_op = get_operations_by_name("Spectrum::ClassifyPeakIntensities(0)", $operation['id']);
        echo "<tr><td><a href=\"dump.php?id=".$filter_op[0]['id']."\">Classify peaks</a></td><td>";
        echo_spectrum($filter_op[0], "*this/peakData", "*this end");
        echo "</td></tr>";

        $filter_op = get_operations_by_name("Spectrum::NormalizePeakIntensities(0)", $operation['id']);
        echo "<tr><td><a href=\"dump.php?id=".$filter_op[0]['id']."\">Normalize peaks (for XCorr)</a></td><td>";
        echo_spectrum($filter_op[0], "*this/peakData", "*this end", "no classes");
        echo "</td></tr>";

        echo "</table>";
    }
}

function summary_proteins() {

}

function summary_all_ops() {
    $i = 0;
    $ops = get_all_ops();
    $color_per_name = array();
    srand(0);
    foreach($ops as $op) {
        $i++;
        echo str_pad($i, 6);
        $name = $op['name'];
        if(!array_key_exists($name, $color_per_name)) {
            $rand = array('0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a');
            $color = '#'.$rand[rand(0,10)].$rand[rand(0,10)].$rand[rand(0,10)].$rand[rand(0,10)].$rand[rand(0,10)].$rand[rand(0,10)];
            $color_per_name[$name] = $color;
        } else {
            $color = $color_per_name[$name];
        }
        echo str_repeat("   ", $op['depth'])."<a href=\"dump.php?id=".$op['id']."\" style=\"color: $color\">".$op['name']."</a>\n";
    }
}

function summary_all_proteins() {
    $ops = get_operations_by_name("all(0)/process protein(*)");
    $count = 0;
    foreach($ops as $op) {
        $proteinName = get_io_by_name($op['id'], "p.getName()", "protein name");
        echo "&gt;<a href=\"dump.php?id=".$op['id']."\">" . $proteinName[0]['value']."</a><br/>";
        $proteinSequence = get_io_by_name($op['id'], "protein/sequence()", "protein to query");

        $str = $proteinSequence[0]['value'];
        $str2 = chunk_split($str, 10, ' ');
        $str2 = chunk_split($str2, 88);
        echo "<pre>$str2</pre><br/><br/>";
        $count++;
    }
}

function summary_search_results($spectrum_id, $protein_name) {
    $ops = get_operations_by_name("all(0)/process protein(*)");
    foreach($ops as $op) {
        $proteinName = get_io_by_name($op['id'], "p.getName()", "protein name");
        echo "<h4><a href=\"dump.php?id=".$op['id']."\">" . $proteinName[0]['value']."</a></h4>";

        $scores = get_operations_by_name("query digested peptides(*)/query digested peptide(*)/query all ptm variants(*)/QuerySequence(*)/test charge(*)/match candidate hypothesis(*)/Spectrum::ScoreSequenceVsSpectrum(*)", $op['id']);
        foreach($scores as $score) {

            $seq_io = get_io_by_name($score['id'], "seq", "Sequence to match");

            echo "<a href=\"dump.php?id=".$score['id']."\">".$seq_io[0]['value']."</a>";
            #echo $seq_io[0]['value'];
            #var_dump($score);
            echo "<br/>";
        }

//        $proteinSequence = get_io_by_name($op['id'], "protein/sequence()", "protein to query");
//        $str = $proteinSequence[0]['value'];
//        $str2 = chunk_split($str, 10, ' ');
//        $str2 = chunk_split($str2, 88);
//        echo "<pre>$str2</pre><br/><br/>";
    }
}
