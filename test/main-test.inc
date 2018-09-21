//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int, char**)
{
  // Informs test listeners about test results
  CPPUNIT_NS::TestResult testresult;

  // Registering listeners to collect test results
  CPPUNIT_NS::TestResultCollector collectedresults;
  testresult.addListener (&collectedresults);

  // Listener to output the results of individual tests
  CPPUNIT_NS::BriefTestProgressListener progress;
  testresult.addListener (&progress);

  // Inserting a test suite via the registry in the test-runner
  CPPUNIT_NS::TestRunner testrunner;
  testrunner.addTest (CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
  testrunner.run (testresult);

  // Outputting results in compiler format
  CPPUNIT_NS::CompilerOutputter compileroutputter ( &collectedresults
                                                  , std::cerr );
  compileroutputter.write();

  // Feedback whether tests were successful
  return collectedresults.wasSuccessful() ? 0 : 1;
}

