import numpy as _np
import os as _os
import string as _string
import glob as _glob
import subprocess as _sub
import multiprocessing
import time
import collections

import Globals
import PhaseSpace
import Writer
import TestResults
from pybdsim import Writer as _pybdsimWriter
from pybdsim import Options as _options

# data type with multiple entries that can be handled by the functions.
multiEntryTypes = [tuple, list, _np.ndarray]

GlobalData = Globals.Globals()


def Run(inputDict):
    """ Generate the rootevent output for a given gmad file.
        """
    inputfile = inputDict['testfile']
    isSelfComparison = inputDict['isSelfComparison']
    generateOriginal = inputDict['generateOriginal']

    # strip extension to leave test name, will be used as output name
    if inputfile[-5:] == '.gmad':
        outputfile = inputfile[:-5]
    else:
        outputfile = inputfile
    outputfile = outputfile.split('/')[-1]

    bdsimLogFile = outputfile + "_bdsim.log"
    comparatorLogFile = outputfile + "_comp.log"

    inputDict['bdsimLogFile'] = bdsimLogFile
    inputDict['compLogFile'] = comparatorLogFile

    t = time.time()

    # run bdsim and dump output to temporary log file.
    # os.system used as subprocess.call has difficulty with the arguments for some reason.
    bdsimCommand = GlobalData._bdsimExecutable + " --file=" + inputfile + " --output=rootevent --outfile="
    bdsimCommand += outputfile + " --batch"
    _os.system(bdsimCommand + ' > ' + bdsimLogFile)
    bdsimTime = _np.float(time.time() - t)
    inputDict['bdsimTime'] = bdsimTime

    # quick check for output file. If it doesn't exist, update the main failure log and return None.
    # If it does exist, delete the log and return the filename
    files = _glob.glob('*.root')
    testOutputFile = outputfile + '_event.root'
    inputDict['ROOTFile'] = testOutputFile

    if not files.__contains__(testOutputFile):
        # outputLog = open(comparatorLogFile, 'a')  # temp log file for the comparator output.
        # outputLog.write('\r\n')
        # outputLog.write('Output from ' + inputfile + ' was not written.')
        # outputLog.close()
        inputDict['code'] = GlobalData.returnCodes['FILE_NOT_FOUND']  # False
    elif not generateOriginal:
        outputLog = open(comparatorLogFile, 'a')  # temp log file for the comparator output.

        # Only compare if the output was generated.
        if isSelfComparison:
            originalFile = testOutputFile.split('_event.root')[0] + '_event2.root'
            copyString = 'cp ' + testOutputFile + ' ' + originalFile
            _os.system(copyString)
        else:
            originalFile = inputDict['originalFile']

        #run comparator and record comparator time.
        t = time.time()
        outputLog.write('\r\n')
        TestResult = _sub.call(args=[GlobalData._comparatorExecutable, originalFile, testOutputFile], stdout=outputLog)
        outputLog.close()
        ctime = time.time() - t
        inputDict['compTime'] = ctime

        # immediate pass/fail bool for decision on keeping root output
        hasPassed = True
        if TestResult != 0:
            hasPassed = False

        if hasPassed:
            _os.system("rm " + testOutputFile)
            if isSelfComparison:
                _os.system("rm " + originalFile)
            inputDict['code'] = GlobalData.returnCodes['SUCCESS']  # True
            _os.system("rm " + comparatorLogFile)
        else:
            _os.system("mv " + testOutputFile + " FailedTests/" + testOutputFile)  # move the failed file
            _os.system("mv " + comparatorLogFile + " FailedTests/" + comparatorLogFile)  # move failed comp. log file
            _os.system("mv " + bdsimLogFile + " FailedTests/" + bdsimLogFile)  # move failed bdsim log file

            if isSelfComparison:
                _os.system("rm " + originalFile)
            inputDict['code'] = GlobalData.returnCodes['FAILED']  # False
    elif generateOriginal:
        inputDict['code'] = GlobalData.returnCodes['SUCCESS']
        _os.remove(bdsimLogFile)
    return inputDict


class Test(dict):
    def __init__(self, component, energy, particle, phaseSpace=None,
                 useDefaults=False, testRobustNess=False, eFieldMap='',
                 bFieldMap='', comparisonFile='', **kwargs):

        dict.__init__(self)
        self._numFiles = 0
        self._testFiles = []
        self._useDefaults = useDefaults
        self._testRobustness = testRobustNess
        self.PhaseSpace = None
        self._beamFilename = 'trackingTestBeam.madx' # default file name
        
        # Initialise parameters for the component as empty lists (or defaults) and dynamically
        # create setter functions for those component parameters.
        if isinstance(component, _np.str) and (GlobalData.components.__contains__(component)):
            self.Component = component
            for param in GlobalData.hasParams[component]:
                if not useDefaults:
                    self[param] = []
                else:
                    self.__Update(param, GlobalData.paramValues[param])
                funcName = "Set" + _string.capitalize(param)
                setattr(self, funcName, self.__createSetterFunction(name=param))
            
            # set the parameter values to the kwarg values if defaults are not specified
            if not useDefaults:
                for key, value in kwargs.iteritems():
                    if GlobalData.hasParams[component].__contains__(key):
                        self.__Update(key, value)
        else:
            raise ValueError("Unknown component type.")

        self.SetComparisonFilename(fileName=comparisonFile)
        self.SetEnergy(energy)
        self.SetBeamPhaseSpace(phaseSpace)
        self.SetParticleType(particle)

    def __createSetterFunction(self, name=''):
        """ Function to return function template for updating component parameters.
            """
        def function_template(value):
            self.__Update(name, value)
        return function_template

    def __Update(self, parameter, values):
        """ Function to check the data type is allowable and set dict[parameter] to be those values.
            This allows the input to be multiple data types.
            
            Also, the number of parameter test combinations is recalculated upon the dict being updated.
            """
        # function to test if entry is numeric. note: is_numlike doesn't work on strings e.g. '3',
        # which we can accept as input, hence this function.
        def _isNumeric(number):
            try:
                floatval = _np.float(number)
            except ValueError:
                raise ValueError("Cannot set "+parameter+" to '"+val+"'.")
            return floatval

        variableValues = []
        # process multiple value types
        if multiEntryTypes.__contains__(type(values)):
            if len(values) > 0:
                for val in values:
                    # if each value is another multiple entry type, set it as the parameter value
                    if multiEntryTypes.__contains__(type(val)):
                        variableValues.append(val)
                    else:
                        # try converting all other dtypes to float
                        val = _isNumeric(val)
                        variableValues.append(val)
                self[parameter] = variableValues
            else:
                pass
        else:
            val = _isNumeric(values)
            variableValues.append(val)
            self[parameter] = variableValues
        self._UpdateTestCombinations()

    def _UpdateTestCombinations(self):
        # update number of parameter combinations
        numcomponentVariations = 1
        for key, values in self.iteritems():
            if key == 'knl' or key == 'ksl':
                numcomponentVariations *= (len(GlobalData.k1l)*len(values))
            elif len(values) != 0:
                numcomponentVariations *= len(values)
        if (self.Component == 'rbend' or self.Component == 'sbend') and self._useDefaults:
            numcomponentVariations /= len(GlobalData.paramValues['field'])
            numcomponentVariations *= 2  # angle or field
        self._numFiles = numcomponentVariations

    def __repr__(self):
        s = 'pybdsimTest.Test instance.\r\n'
        s += 'This is a test for a/an ' + self.Component + ' with ' + self.Particle
        s += ' at an energy of ' + _np.str(self.Energy) + ' GeV.\r\n'
        s += 'The component will be test all combinations of the following parameters:\r\n'
        for param in self.keys():
            s += '  '+param+' : ' + self[param].__repr__()+'\r\n'
        return s

    def SetComparisonFilename(self, fileName=''):
        """ Function to set the filename to which the generated BDSIM output will be compared.
            This filename can only be set if the pybdsim.Test object represent a solitary test.
            """
        if (self._numFiles > 1) and (fileName != ''):
            s = "This pybdsim.Test object is set to generate " + _np.str(self._numFiles) + " output files.\r\n"
            s += "A comparison test file can only be specified if this object represents a solitary test.\r\n"
            print(s)
        elif isinstance(fileName, _np.str) and (fileName != ''):
            # only set filename if the file exists
            startOfFileName = fileName.rfind('/')
            if startOfFileName == -1:
                files = _glob.glob('*')
                fName = startOfFileName
            else:
                files = _glob.glob(fileName[:(startOfFileName + 1)] + '*')
                fName = fileName[(startOfFileName + 1):]
            if files.__contains__(fName):
                self._testFiles = fileName

    def SetEnergy(self, energy):
        """ Set test beam energy.
            """
        try:
            setattr(self, "Energy", _np.float(energy))
        except TypeError:
            raise ValueError("Unknown data type.")

    def SetParticleType(self, particle=''):
        """ Set test beam particle.
            """
        if GlobalData.particles.__contains__(particle):
            setattr(self, "Particle", particle)
        else:
            raise ValueError("Unknown particle type")

    def SetBeamPhaseSpace(self, phaseSpace=None, x=0, px=0, y=0, py=0, t=0, pt=0):
        """ Set beam phase space. optional phaseSpace arg must be
            a PhaseSpace instance. Entry of particle co-ordinates instead
            internally creates a PhaseSpace instance anyway."""
        if phaseSpace is not None:
            if isinstance(phaseSpace, PhaseSpace.PhaseSpace):
                self.PhaseSpace = phaseSpace
            else:
                raise TypeError("phaseSpace can only be a bdsimtesting.PhaseSpace.PhaseSpace instance.")
        else:
            self.PhaseSpace = PhaseSpace.PhaseSpace(x, px, y, py, t, pt)

    def SetInrays(self, inraysFile=''):
        if inraysFile != '':
            self._beamFilename = inraysFile

    def AddParameter(self, parameter, values=[]):
        if self.keys().__contains__(parameter):
            raise ValueError("Parameter is already listed as a test parameter.")
        
        elif isinstance(parameter, _np.str):
            if GlobalData.parameters.__contains__(parameter):
                self[parameter] = []
                funcName = "Set" + _string.capitalize(parameter)
                setattr(self, funcName, self.__createSetterFunction(name=parameter))
                self.__Update(parameter, values)
            else:
                raise ValueError("Unknown parameter type: " + parameter + ".")
        else:
            raise TypeError("Unknown data type for " + parameter)

    def WriteToInrays(self, filename):
        self.SetInrays(filename)
        self.PhaseSpace._WriteToInrays(filename)


class TestUtilities(object):
    def __init__(self, testingDirectory='', dataSetDirectory=''):
        self._tests = []  # list of test objects
        self._testNames = {}  # dict of test file names (a list of names per component)

        if not isinstance(testingDirectory, _np.str):
            raise TypeError("Testing directory is not a string")
        else:
            if testingDirectory == '':
                pass
            elif _os.getcwd().split('/')[-1] == testingDirectory:
                pass
            # check for directory and make it if not:
            elif _os.path.exists(testingDirectory):
                _os.chdir(testingDirectory)
            elif not _os.path.exists(testingDirectory):
                _os.system("mkdir " + testingDirectory)
                _os.chdir(testingDirectory)

        # make dirs for gmad files, bdsimoutput, and failed outputs
        if not _os.path.exists('Tests'):
            _os.system("mkdir Tests")

        if not _os.path.exists('BDSIMOutput'):
            _os.system("mkdir BDSIMOutput")
        _os.chdir('BDSIMOutput')
        if not _os.path.exists('FailedTests'):
            _os.system("mkdir FailedTests")
        _os.chdir('../')

        self._dataSetDirectory = ''
        if isinstance(dataSetDirectory, _np.str):
            self._dataSetDirectory = dataSetDirectory

        self.bdsimFailLog = 'bdsimOutputFailures.log'
        self.bdsimPassLog = 'bdsimOutputPassed.log'
        self._comparatorLog = 'comparatorOutput.log'

    def WriteGmadFiles(self):
        """ Write the gmad files for all tests in the Tests directory.
            """
        _os.chdir('Tests')
        for test in self._tests:
            writer = Writer.Writer()
            writer.WriteTests(test)
            if not self._testNames.__contains__(test.Component):
                self._testNames[test.Component] = []
            self._testNames[test.Component].extend(writer._fileNamesWritten[test.Component])
        _os.chdir('../')
    
    def GenerateRootFile(self, inputfile):
        """ Generate the rootevent output for a given gmad file.
            """
        # strip extension to leave test name, will be used as output name
        if inputfile[-5:] != '.gmad':
            outputfile = inputfile[:-5]
        else:
            outputfile = inputfile
        outputfile = outputfile.split('/')[-1]

        # run bdsim and dump output to temporary log file.
        # os.system used as subprocess.call has difficulty with the arguments for some reason.
        bdsimCommand = GlobalData._bdsimExecutable + " --file=" + inputfile + " --output=rootevent --outfile="
        bdsimCommand += outputfile + " --batch"
        _os.system(bdsimCommand + ' > temp.log')

        # quick check for output file. If it doesn't exist, update the main failure log and return None.
        # If it does exist, delete the log and return the filename
        files = _glob.glob('*.root')
        outputevent = outputfile + '_event.root'
        if not files.__contains__(outputevent):
            return None
        else:
            _os.system("rm temp.log")
            return outputevent

    def CompareOutput(self, originalFile='', newFile='', isSelfComparison=False):
        """ Compare the output file against another file. This function uses BDSIM's comparator.
            The test gmad file name is needed for updating the appropriate log files.
            If the comparison is successful:
                The generated root file is deleted.
                The BDSIM pass log file is updated.
            If the comparison is not successful:
                The generated file is moved to the FailedTests directory.
                The Comparator log is updated.
            """

        outputLog = open("tempComp.log", 'w')  # temp log file for the comparator output.
        TestResult = _sub.call(args=[GlobalData._comparatorExecutable, originalFile, newFile], stdout=outputLog)
        outputLog.close()

        # immediate pass/fail bool for decision on keeping root output
        hasPassed = True
        if TestResult != 0:
            hasPassed = False

        if hasPassed:
            # _os.system("rm "+newFile)
            if isSelfComparison:
                _os.system("rm " + originalFile)
            _os.system("rm tempComp.log")
            f = open(self.bdsimPassLog, 'a')  # Update the pass log
            f.write("File " + newFile + " has passed.\r\n")
            f.close()
        else:
            if isSelfComparison:
                _os.system("rm " + originalFile)
            _os.system("mv " + newFile + " FailedTests/" + newFile)  # move the failed file
            self._UpdateComparatorLog(newFile)
            _os.system("rm tempComp.log")

    def _UpdateBDSIMFailLog(self, testFileName):
        """ Update the test failure log.
            """
        f = open('temp.log', 'r')
        g = open(self.bdsimFailLog, 'a')
        g.write('\r\n')
        g.write('FAILED TEST FILE: ' + testFileName)
        g.write('\r\n')
        for line in f:
            g.write(line)
        g.close()

    def _UpdateComparatorLog(self, testFileName):
        """ Update the test pass log.
            """
        f = open('tempComp.log', 'r')
        g = open(self._comparatorLog, 'a')
        g.write('\r\n')
        g.write('FAILED TEST FILE: ' + testFileName)
        g.write('\r\n')
        for line in f:
            g.write(line)
        g.close()

    def WriteGlobalOptions(self):
        """ Write the options file that will be used by all test files.
            """
        options = _options.Options()
        options.SetSamplerDiameter(3)
        options.SetWritePrimaries(False)
        options.SetPhysicsList(physicslist="em hadronic")
        writer = _pybdsimWriter.Writer()
        writer.WriteOptions(options, 'Tests/trackingTestOptions.gmad')

    def _CheckForOriginal(self, testname, componentType):
        if self._dataSetDirectory != '':
            dataDir = self._dataSetDirectory
        else:
            dataDir = 'OriginalDataSet'
        testname = testname.replace('Tests', dataDir)
        testname = testname.replace('.gmad', '_event.root')
        files = _glob.glob('../' + dataDir + '/' + componentType + "/*.root")
        if files.__contains__(testname):
            return testname
        else:
            return ''

    def _GetOrderedTests(self, testlist, componentType):
        OrderedTests = []
        particles = []
        compKwargs = collections.OrderedDict()  # keep order of kwarg in file name

        for test in testlist:
            fnameStartIndex = test.rfind('/')
            filename = test[fnameStartIndex + 1:]
            path = test[:fnameStartIndex+1]
            if filename[-5:] == '.gmad':
                filename = filename[:-5]  # remove .gmad extension
            splitFilename = filename.split('__')
            particle = splitFilename[1]
            if not particles.__contains__(particle):
                particles.append(particle)

            # dict of compiled lists of different kwarg values.
            for kwarg in splitFilename[2:]:
                param = kwarg.split('_')[0]
                value = kwarg.split('_')[1]
                if not compKwargs.keys().__contains__(param):
                    compKwargs[param] = []
                if not compKwargs[param].__contains__(value):
                    compKwargs[param].append(value)
            # sort the kwarg values
            for key in compKwargs.keys():
                compKwargs[key].sort()

        kwargKeys = compKwargs.keys()

        # recursively create filenames from all kwarg value permutations.
        # if filename matches one in supplied test list, add to ordered list.
        def sublevel(depth, nameIn):
            for value in compKwargs[kwargKeys[depth]]:
                name = nameIn + '__' + kwargKeys[depth] + "_" + value
                if depth < (len(kwargKeys) - 1):
                    sublevel(depth + 1, name)
                elif testlist.__contains__(path + name + '.gmad'):
                    OrderedTests.append(path + name + '.gmad')

        for particle in particles:
            fname = componentType + "__" + particle
            sublevel(0, fname)
        return OrderedTests


class TestSuite(TestUtilities):
    def __init__(self, testingDirectory, dataSetDirectory='', _useSingleThread=False):
        super(TestSuite, self).__init__(testingDirectory, dataSetDirectory)
        self._useSingleThread = _useSingleThread
        self._generateOriginals = False  # bool for generating original data set
        self.Results = TestResults.Results()  # results instance
        self.timings = TestResults.Timing()  # timing data.

    def AddTest(self, test):
        """ Add a bdsimtesting.pybdsimTest.Test instance to the test suite.
            """
        if not isinstance(test, Test):
            raise TypeError("Test is not a bdsimtesting.pybdsimTest.Test instance")
        else:
            self._tests.append(test)

    def GenerateOriginals(self):
        """ Function to generate an original data set.
            """
        # change class attribute here, generate data, and change back.
        # It's safer to not pass the bool in as an arg, it could be accidentally
        # passed and potentially overwrite an existing data set.
        self._generateOriginals = True
        if not _os.path.exists('OriginalDataSet'):
            _os.system("mkdir OriginalDataSet")
        self.RunTestSuite()
        self._generateOriginals = False
        _os.chdir('OriginalDataSet')
        self.Results.ProcessOriginals()
        _os.chdir('../')

    def RunTestSuite(self, isSelfComparison=False):
        """ Run all tests in the test suite. This will generate the tests rootevent
            output, compares to an original file, and processes the comparison results.
            """
        self.WriteGlobalOptions()
        self.WriteGmadFiles()   # Write all gmad files for all test objects.

        if self._generateOriginals:
            _os.chdir('OriginalDataSet')
        else:
            _os.chdir('BDSIMOutput')

        initialTime = time.time()

        for componentType in self._testNames.keys():

            testfilesDir = '../Tests/'

            if self._generateOriginals:
                if not _os.path.exists(componentType):
                    _os.system("mkdir "+componentType)
                _os.chdir(componentType)
                testfilesDir = '../../Tests/'

            t = time.time()  # initial time

            tests = []
            for testFile in self._testNames[componentType]:
                tests.append(testfilesDir + testFile)

            if len(tests) > 1:
                tests = self._GetOrderedTests(tests, componentType)

            # compile iterable list of dicts for multithreading function.
            testlist = []
            for test in tests:
                origname = self._CheckForOriginal(test, componentType)

                # pass data in a dict. Easier to pass single expandable variable.
                testDict = {'testfile'          : test,
                            'componentType'     : componentType,
                            'originalFile'      : origname,
                            'bdsimLogfile'      : '',
                            'compLogFile'       : '',
                            'ROOTFile'          : '',
                            'generateOriginal'  : self._generateOriginals,
                            'isSelfComparison'  : isSelfComparison,
                            'bdsimTime'         : 0,
                            'compTime'          : 0,
                            'code'              : None
                            }
                testlist.append(testDict)

            if not self._useSingleThread:
                self._multiThread(testlist)  # multithreaded option
            else:
                self._singleThread(testlist)  # single threaded option.

            componentTime = time.time() - t  # final time
            self.timings.AddComponentTime(componentType, componentTime)

            if self._generateOriginals:
                _os.chdir('../')
            else:
                self.Results.ProcessResults(componentType=componentType)

        finalTime = time.time() - initialTime
        self.timings.SetTotalTime(finalTime)
        _os.chdir('../')

        self.Results.AddTimingData(self.timings)

    def _multiThread(self, testlist):
        numCores = multiprocessing.cpu_count()

        p = multiprocessing.Pool(numCores)
        results = p.map(Run, testlist)

        for testRes in results:
            self.Results.AddResults(testRes)
            self.timings.bdsimTimes.append(testRes['bdsimTime'])
            self.timings.comparatorTimes.append(testRes['compTime'])

    def _singleThread(self, testlist):
        eleBdsimTimes = []
        eleComparatorTimes = []

        for testDict in testlist:
            test = testDict['file']
            isSelfComparison = testDict['isSelfComparison']
            originalEvent = testDict['originalFile']

            bdsimTestTime = time.time()
            outputEvent = self.GenerateRootFile(test)
            bdsimTime = time.time() - bdsimTestTime
            eleBdsimTimes.append(bdsimTime)
            compTestTime = time.time()
            # Only compare if the output was generated.

            if (outputEvent is not None) and (not self._generateOriginals):
                if isSelfComparison:
                    originalEvent = outputEvent.split('_event.root')[0] + '_event2.root'
                    copyString = 'cp ' + outputEvent + ' ' + originalEvent
                    _os.system(copyString)
                else:
                    pass  # This is where the comparison with the original file will occur.
                    # TODO: figure out how to process original files that will be compared to.

                self.CompareOutput(originalEvent, outputEvent)
            else:
                self._UpdateBDSIMFailLog(test)
                _os.system("rm temp.log")
                print("Output for test " + test + " was not generated.")
            comparatorTime = time.time() - compTestTime
            eleComparatorTimes.append(comparatorTime)

        self.timings.bdsimTimes.extend(_np.average(eleBdsimTimes))
        self.timings.comparatorTimes.extend(_np.average(eleComparatorTimes))

    def _FullTestSuite(self):
        writer = Writer.Writer()
        writer.SetBeamFilename('trackingTestBeam.madx')
        writer.SetOptionsFilename('trackingTestOptions.gmad')

        self.numFiles = {}
        self.componentTests = []
        for component in GlobalData.components:
            self.numFiles[component] = 0
        
        TestPS = GlobalData.BeamPhaseSpace
    
        BeamPhaseSpace = PhaseSpace.PhaseSpace(TestPS['X'], TestPS['PX'], TestPS['Y'], TestPS['PY'], TestPS['T'], TestPS['PT'])
        BeamPhaseSpace.WriteToInrays('Tests/trackingTestBeam.madx')

        for machineInfo in GlobalData.accelerators.values():
            energy = machineInfo['energy']
            particle = machineInfo['particle']
        
            for component in GlobalData.components:
                componentTest = Test(component, energy, particle, BeamPhaseSpace, useDefaults=True)
                self.componentTests.append(componentTest)
                self.numFiles[component] += componentTest._numFiles

        self.totalFiles = 0
        for component in self.numFiles:
            self.totalFiles += self.numFiles[component]

        self.BeamPhaseSpace = BeamPhaseSpace


