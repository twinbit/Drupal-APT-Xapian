<?php

function apt_xabian_db($path = '') {
   (!$oath ? $path = variable_get('xapian_db_path', '') : '');
   if (isset($path) && !file_exists($path)) {
     $error = 'Xapian db invalid path.';
     drupal_set_message($error, 'error');
     watchdog('apt_xapian', $error);
     return false;
   }
   else {
      try {
        $database = new XapianDatabase($path);
        return $database;
      }
      catch (Exception $e) {
        $error = 'Xapian Engine Error:'  . $e->getMessage();
        drupal_set_message($error, 'error');
        watchdog('apt_xapian', $error);
        return false;
      }
   }
}


function apt_xapian_query($s, $limit = 20) {
  $database = apt_xabian_db();
  if ($database) {
    try {
      // Start an enquire session.
      $enquire = new XapianEnquire($database);

      // Combine the rest of the command line arguments with spaces between
      // them, so that simple queries don't have to be quoted at the shell
      // level.
      $query_string = $s;
    
      $qp = new XapianQueryParser();
      $stemmer = new XapianStem("english");
      $qp->set_stemmer($stemmer);
      $qp->set_database($database);
      $qp->set_stemming_strategy(XapianQueryParser::STEM_SOME);
      $query = $qp->parse_query($query_string);

      // Find the top 10 results for the query.
      $enquire->set_query($query);
      $matches = $enquire->get_mset(0, $limit);
      return $matches;
    }
    catch (Exception $e) {
      return $e->getMessage();
    }
  }
}
