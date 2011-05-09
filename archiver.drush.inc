<?php
/**
 * @file
 * Drush command to extract archives.
 *
 * Most parts of the code rely fully on the archiver implementation of D7.
 * @link http://api.drupal.org/api/search/7/archiver
 *
 * Code of the drupal archiver and addtional parts base on the Tar / Zip
 * libraries of Vincent Blavet <vincent@phpconcept.net> from PhpConcept.
 *
 * @todo Integrate fallback code for zip handling -> system.zip.inc
 */

/**
 * Implements EXTENSION_drush_init().
 */
function archiver_drush_init() {
  // Load the interface class.
  require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'archiver.inc';
  // Load the interface implementaions. Libraries are loaded later.
  require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'system.archiver.inc';
}

/**
 * Implements hook_drush_command().
 */
function  archiver_drush_command() {
  $items['archiver-extract'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'description' => 'Extracts an archive into a specified location. The used compression is detected automatically.',
    'arguments' => array(
      'archive' => 'Filename of the archive to extract.',
      'location' => 'The path at which to extract the archive.',
    ),
    'examples' => array(
      'drush archiver-extract /tmp/drupal-7.0.tar.gz /var/www/my_drupal_site' => 'Extract the drupal archive into the the directory /var/www/my_drupal_site.',
      'drush aextract /tmp/drupal-7.0.tar.gz /var/www/my_drupal_site' => 'Extract the drupal archive into the the directory /var/www/my_drupal_site.',
      'drush archiver-extract /tmp/drupal-7.0.tar.gz /var/www/my_drupal_site --compression=tar.gz' => 'Extract the drupal archive into the the directory /var/www/my_drupal_site.',
    ),
    'aliases' => array('aextract'),
  );

  $items['archiver-compress'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'description' => 'Create or update an archive. The compression is detected automatically from the archive extension.',
    'arguments' => array(
      'archive' => 'Filename of the archive to create / update. Include the archive extension to use the compression autodetection.',
      'files...' => 'The path of the file or directory to add.',
    ),
    'examples' => array(
      'drush archiver-compress /tmp/my_durpal_site.tar.gz /var/www/my_drupal_site' => 'Compress the folder /var/www/my_drupal_site and create the archive /tmp/my_durpal_site.tar.gz.',
    ),
    'aliases' => array('acompress'),
  );

  return $items;
}

/**
 * Implementation of hook_drush_help().
 */
function archiver_drush_help($section) {
  switch ($section) {
    case 'drush:archiver-extract':
      return dt('Extracts an archive into a specified location. The used compression is detected automatically.');
    case 'drush:archiver-compress':
      return dt('Create an archive. The compression is detected automatically from the file extension of the archive.');
  }
}

/**
 * Command callback. Extract archive into location.
 */
function drush_archiver_extract($archive, $location) {
  try {
    if (!$archiver = drush_archiver_get_archiver($archive)) {
      drush_set_error('Archiver', dt('No matching archiver found for archive %archive.', array('%archive' => $archive)));
      return FALSE;
    }
    drush_log(dt('Extract %archive to %location.', array('%archive' => $archive, '%location' => $location)), 'ok');
    drush_log(dt('Use archiver %class.', array('%class' => get_class($archiver))), 'ok');
    $archiver->extract($location);
  } catch (Exception $e) {
    drush_set_error($e->getCode(), (string) $e);
  }
}

/**
 * Command callback. Add location to archive.
 */
function drush_archiver_compress($archive) {
  $files = func_get_args();
  array_shift($files);

  try {
    if (!$archiver = drush_archiver_get_archiver($archive)) {
      drush_set_error('Archiver', dt('No matching archiver found for archive %archive.', array('%archive' => $archive)));
      return FALSE;
    }
    drush_log(dt('Use archiver %class.', array('%class' => get_class($archiver))), 'ok');
    foreach ($files as $location) {
      if (!file_exists($location)) {
        drush_set_error('FILE_NOT_FOUND', dt('File to compress %location not found.', array('%location' => $location)));
        continue;
      }
      drush_log(dt('Add %location to %archive.', array('%archive' => $archive, '%location' => $location)), 'ok');
      $archiver->add($location);
    }
  } catch (Exception $e) {
    drush_set_error($e->getCode(), (string) $e);
  }
}

// To be able to copy/paste the archiver stuff emulate some drupal functions
if (!function_exists('drupal_mkdir')) {
  /**
   * @link http://api.drupal.org/api/drupal/includes--file.inc/function/drupal_mkdir/7
   */
  function drupal_mkdir($uri, $mode = NULL, $recursive = FALSE, $context = NULL) {
    if (!isset($mode)) {
      $mode = 0775;
    }
    if (!isset($context)) {
      return mkdir($uri, $mode, $recursive);
    }
    else {
      return mkdir($uri, $mode, $recursive, $context);
    }
  }

  /**
   * @link http://api.drupal.org/api/drupal/includes--file.inc/function/drupal_unlink/7
   */
  function drupal_unlink($path) {
    return drush_delete_dir($path);
  }
}

/**
 * Returns a string of supported archive extensions.
 *
 * @return
 *   A space-separated string of extensions suitable for use by the file
 *   validation system.
 */
function drush_archiver_get_extensions() {
  $valid_extensions = array();
  $archiver_info = drush_archiver_compression_info();
  foreach ($archiver_info as $archive) {
    foreach ($archive['extensions'] as $extension) {
      foreach (explode('.', $extension) as $part) {
        if (!in_array($part, $valid_extensions)) {
          $valid_extensions[] = $part;
        }
      }
    }
  }
  return implode(' ', $valid_extensions);
}


/**
 * Returns the archiver matching to the compression.
 *
 * @param string $filepath
 *   The full path of the archive file. Note that stream wrapper paths are
 *   supported, but not remote ones.
 * @return ArchiverInterface
 *   A newly created instance of the archiver class appropriate
 *   for the specified file, already bound to that file.
 *   If no appropriate archiver class was found, will return FALSE.
 */
function drush_archiver_get_archiver($filepath) {

  // Archivers can only work on local paths
  if (is_file($filepath)) {
    $filepath = realpath($filepath);
    if (!is_file($filepath)) {
      throw new Exception(dt('Archivers can only operate on local files: %file not supported', array('%file' => $filepath)));
    }
  }

  $archiver_info = drush_archiver_compression_info();

  foreach ($archiver_info as $implementation) {
    foreach ($implementation['extensions'] as $extension) {
      // Because extensions may be multi-part, such as .tar.gz,
      // we cannot use simpler approaches like substr() or pathinfo().
      // This method isn't quite as clean but gets the job done.
      // Also note that the file may not yet exist, so we cannot rely
      // on fileinfo() or other disk-level utilities.
      if (strrpos($filepath, '.' . $extension) === strlen($filepath) - strlen('.' . $extension)) {
        require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . $implementation['file'];
        return new $implementation['class']($filepath);
      }
    }
  }
  return FALSE;
}


/**
 * Returns archiver information.
 */
function drush_archiver_compression_info() {
  $archiver_info['tar'] = array(
    'class' => 'ArchiverTar',
    'extensions' => array('tar', 'tgz', 'tar.gz', 'tar.bz2', 'gz'),
    'file' => 'system.tar.inc',
  );
  if (function_exists('zip_open')) {
    $archiver_info['zip'] = array(
      'class' => 'ArchiverZip',
      'extensions' => array('zip'),
      'file' => 'system.zip.inc',
    );
  }
  return $archiver_info;
}
