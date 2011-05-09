<?php
// $Id: system.archiver.inc,v 1.6 2010/12/30 22:33:04 webchick Exp $

/**
 * @file
 * Archiver implementations provided by the system module.
 */

/**
 * Archiver for .tar files.
 */
class ArchiverTar implements ArchiverInterface {

  /**
   * The underlying Archive_Tar instance that does the heavy lifting.
   *
   * @var Archive_Tar
   */
  protected $tar;

  public function __construct($file_path) {
    $this->tar = new Archive_Tar($file_path);
  }

  public function add($file_path) {
    $this->tar->add($file_path);

    return $this;
  }

  public function remove($file_path) {
    // @todo Archive_Tar doesn't have a remove operation
    // so we'll have to simulate it somehow, probably by
    // creating a new archive with everything but the removed
    // file.

    return $this;
  }

  public function extract($path, Array $files = array()) {
    if ($files) {
      $this->tar->extractList($files, $path);
    }
    else {
      $this->tar->extract($path);
    }

    return $this;
  }

  public function listContents() {
    $files = array();
    foreach ($this->tar->listContent() as $file_data) {
      $files[] = $file_data['filename'];
    }
    return $files;
  }

  /**
   * Retrieve the tar engine itself.
   *
   * In some cases it may be necessary to directly access the underlying
   * Archive_Tar object for implementation-specific logic. This is for advanced
   * use only as it is not shared by other implementations of ArchiveInterface.
   *
   * @return
   *   The Archive_Tar object used by this object.
   */
  public function getArchive() {
    return $this->tar;
  }
}

/**
 * Archiver for .zip files.
 *
 * @link http://php.net/zip
 */
class ArchiverZip implements ArchiverInterface {

  /**
   * The underlying ZipArchive instance that does the heavy lifting.
   *
   * @var ZipArchive
   */
  protected $zip;

  public function __construct($file_path) {
    $this->zip = new ZipArchive();
    $flags = NULL;
    // Explicitely set the flags, since Win7 seems to have an issue.
    if (!is_file($file_path)) {
      if (dirname($file_path) && !is_dir(dirname($file_path))) {
        drupal_mkdir(dirname($file_path), NULL, TRUE);
      }
      $flags = ZIPARCHIVE::CREATE;
    }
    if ($this->zip->open($file_path, $flags) !== TRUE) {
      // @todo: This should be an interface-specific exception some day.
      throw new Exception(dt('Cannot open %file_path', array('%file_path' => $file_path)));
    }
  }

  public function add($file_path) {
    $this->zip->addEmptyDir('gnaa');
    $this->zip->addFile($file_path);

    return $this;
  }

  public function remove($file_path) {
    $this->zip->deleteName($file_path);

    return $this;
  }

  public function extract($path, Array $files = array()) {
    if ($files) {
      $this->zip->extractTo($path, $files);
    }
    else {
      $this->zip->extractTo($path);
    }

    return $this;
  }

  public function listContents() {
    $files = array();
    for ($i=0; $i < $this->zip->numFiles; $i++) {
      $files[] = $this->zip->getNameIndex($i);
    }
    return $files;
  }

  /**
   * Retrieve the zip engine itself.
   *
   * In some cases it may be necessary to directly access the underlying
   * ZipArchive object for implementation-specific logic. This is for advanced
   * use only as it is not shared by other implementations of ArchiveInterface.
   *
   * @return
   *   The ZipArchive object used by this object.
   */
  public function getArchive() {
    return $this->zip;
  }

  public function __destruct() {
    if ($this->zip) {
      $this->zip->close();
    }
  }
}
