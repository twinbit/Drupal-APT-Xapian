<?php

function apt_xapian_admin_settings() {
  $form['apt_xapian'] = array(
    '#type' => 'fieldset',
    '#collapsed' => FALSE,
    '#collapsible' => TRUE,
    '#weight' => 0,
    '#title' => t('APT Xapian database settings')
  );

  $form['apt_xapian']['xapian_db_path'] = array(
    '#type' => 'textfield',
    '#title' => t('DB Path'),
    '#description' => t('APT Xapian filesytem path'),
    '#default_value' => variable_get('xapian_db_path', '/var/lib/apt-xapian-index/index')
  );

  return system_settings_form($form);
}

function apt_xapian_admin_settings_validate($form, &$form_state) {
  $path = $form_state['values']['xapian_db_path'];
  if ($path) {
    if (!apt_xabian_db($path)) {
      form_set_error('xapian_db_path', 'Connection not established, please check path.');
    }
  }
}


function apt_xapian_admin_console() {
  $form['apt_xapian_console'] = array(
    '#type' => 'textfield',
    '#title' => t('Query'),
    '#description' => t('Xapian console.'),
    '#default_value' => variable_get('apt_xapian_console', check_plain($_SESSION['_apt_xapian_query']))
  );

  if ($res = $_SESSION['_apt_xapian_console']) {
    $form['result'] = array(
      '#prefix' => '<div class="results"> <h1> Results </h1>',
      '#value' => implode('<br />', $res),
      '#suffix' => '<div>'
    );
    unset($_SESSION['_apt_xapian_console']);
    unset($_SESSION['_apt_xapian_query']);
  }

  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Query')
  );
    return $form;
}

function apt_xapian_admin_console_submit($form, &$form_state) {
  $values = $form_state['values'];
  if ($query = $values['apt_xapian_console']) {
    $res = apt_xapian_query($query);
    if (is_object($res)) {
      if (get_class($res) === 'XapianMSet') {
        $i = $res->begin();
        while (!$i->equals($res->end())) {
          $n = $i->get_rank() + 1;
          $data[] = $i->get_document()->get_data();
          $i->next();
        }
        $_SESSION['_apt_xapian_console'] = $data;
        $_SESSION['_apt_xapian_query'] = $query;
      }
    }
  }
}
