<?php
/**
 * @file
 * Unit tests for drush archiver.
 */

/**
 * Unit tests for drush archiver
 * @TODO Make extendet assertions to make sure the archive contents are valid.
 *
 * @author das-peter <peter.philipp@cando-image.com>
 *
 */
class archiverCase extends Drush_TestCase {

  /**
   * Path to the temp folder for the archiver tests.
   * @var string
   */
  protected $temp_path = NULL;

  /**
   * Path to the test data folder for the archiver tests.
   * @var string
   */
  protected $temp_data_path = NULL;

  /**
   * Set path variables.
   */
  public function setUp() {
    $this->temp_path = UNISH_SANDBOX . DIRECTORY_SEPARATOR . 'archiver' . DIRECTORY_SEPARATOR;
    $this->temp_data_path = dirname(__FILE__) . DIRECTORY_SEPARATOR;
    parent::setUp();
  }

  /**
   * Make sure the folder is clean. Otherwise the artefacts could lead to a
   * false positive.
   */
  public function tearDown() {
    self::file_delete_recursive($this->temp_path);
    parent::tearDown();
  }

  /**
   * Assert that the tar archive extraction works.
   */
  public function testTarExtract() {
    $archive = $this->temp_data_path . 'test.tar';
    $return = $this->execute(UNISH_DRUSH . ' archiver-extract ' . $archive . ' ' . $this->temp_path, self::EXIT_SUCCESS);
    // Check if files exist
    $this->assertTrue(is_file($this->temp_path . 'testfile_1.txt'), 'testfile_1.txt not found');
    $this->assertTrue(is_dir($this->temp_path . 'folder'), 'folder not found');
    $this->assertTrue(is_file($this->temp_path . 'folder/testfile_2.txt'), 'folder/testfile_2.txt not found');
  }

  /**
   * Assert that the tar.gz archive extraction works.
   */
  public function testTarGzExtract() {
    $archive = $this->temp_data_path . 'test.tar.gz';
    $return = $this->execute(UNISH_DRUSH . ' archiver-extract ' . $archive . ' ' . $this->temp_path, self::EXIT_SUCCESS);
    // Check if files exist
    $this->assertTrue(is_file($this->temp_path . 'testfile_1.txt'), 'testfile_1.txt not found');
    $this->assertTrue(is_dir($this->temp_path . 'folder'), 'folder not found');
    $this->assertTrue(is_file($this->temp_path . 'folder/testfile_2.txt'), 'folder/testfile_2.txt not found');
  }

  /**
   * Assert that the tar.bz2 archive extraction works.
   */
  public function testTarBzExtract() {
    $archive = $this->temp_data_path . 'test.tar.bz2';
    $return = $this->execute(UNISH_DRUSH . ' archiver-extract ' . $archive . ' ' . $this->temp_path, self::EXIT_SUCCESS);
    // Check if files exist
    $this->assertTrue(is_file($this->temp_path . 'testfile_1.txt'), 'testfile_1.txt not found');
    $this->assertTrue(is_dir($this->temp_path . 'folder'), 'folder not found');
    $this->assertTrue(is_file($this->temp_path . 'folder/testfile_2.txt'), 'folder/testfile_2.txt not found');
  }

  /**
   * Assert that the zip archive extraction works.
   */
  public function testZipExtract() {
    $archive = $this->temp_data_path . 'test.zip';
    $return = $this->execute(UNISH_DRUSH . ' archiver-extract ' . $archive . ' ' . $this->temp_path, self::EXIT_SUCCESS);
    // Check if files exist
    $this->assertTrue(is_file($this->temp_path . 'testfile_1.txt'), 'testfile_1.txt not found');
    $this->assertTrue(is_dir($this->temp_path . 'folder'), 'folder not found');
    $this->assertTrue(is_file($this->temp_path . 'folder/testfile_2.txt'), 'folder/testfile_2.txt not found');
  }

  /**
   * Assert that an unknown archive throws a proper error.
   */
  public function testUnknownExtract() {
    // File exists but has an unsupported / unknown extension.
    $archive = $this->temp_data_path . 'test.unknown';
    $return = $this->execute(UNISH_DRUSH . ' archiver-extract ' . $archive . ' ' . $this->temp_path, self::EXIT_ERROR);
    // @todo Check why this isn't working. The output seems to be flushed.
    // $this->assertStringStartsWith('No matching archiver found for archive ', $this->getOutput(), 'No proper error message found.');
  }

  /**
   * Assert that the tar archive compression works.
   */
  public function testTarCompress() {
    if (!file_exists($this->temp_path)) {
      mkdir($this->temp_path);
    }
    $archive = $this->temp_path . 'test.tar';
    $files = array(
      $this->temp_data_path . 'testfile_1.txt',
      $this->temp_data_path . 'folder',
    );
    $return = $this->execute(UNISH_DRUSH . ' archiver-compress ' . $archive . ' ' . implode(' ', $files), self::EXIT_SUCCESS);
    // Check if files exist
    $this->assertTrue(is_file($archive), 'Archive not found');
  }

  /**
   * Assert that the tar.gz archive compression works.
   */
  public function testTarGzCompress() {
    if (!file_exists($this->temp_path)) {
      mkdir($this->temp_path);
    }
    $archive = $this->temp_path . 'test.tar.gz';
    $files = array(
      $this->temp_data_path . 'testfile_1.txt',
      $this->temp_data_path . 'folder',
    );
    $return = $this->execute(UNISH_DRUSH . ' archiver-compress ' . $archive . ' ' . implode(' ', $files), self::EXIT_SUCCESS);
    // Check if files exist
    $this->assertTrue(is_file($archive), 'Archive not found');
  }

  /**
   * Assert that the tar.bz2 archive compression works.
   */
  public function testTarBzCompress() {
    if (!file_exists($this->temp_path)) {
      mkdir($this->temp_path);
    }
    $archive = $this->temp_path . 'test.tar.bz2';
    $files = array(
      $this->temp_data_path . 'testfile_1.txt',
      $this->temp_data_path . 'folder',
    );
    $return = $this->execute(UNISH_DRUSH . ' archiver-compress ' . $archive . ' ' . implode(' ', $files), self::EXIT_SUCCESS);
    // Check if files exist
    $this->assertTrue(is_file($archive), 'Archive not found');
  }

  /**
   * Assert that the zip archive compression works.
   * @FIXME It seems like there is an error on some windows systems :| The archive isn't created.
   */
  public function testZipCompress() {
    if (!file_exists($this->temp_path)) {
      mkdir($this->temp_path);
    }
    $archive = $this->temp_path . 'test.zip';
    $files = array(
      $this->temp_data_path . 'testfile_1.txt',
      $this->temp_data_path . 'folder',
    );
    $return = $this->execute(UNISH_DRUSH . ' archiver-compress ' . $archive . ' ' . implode(' ', $files), self::EXIT_SUCCESS);
    // Check if files exist
    $this->assertTrue(is_file($archive), 'Archive not found');
  }
}