// -*-groovy-*-
def gitUrl = "https://gitlab.com/neueda-capopen/sbf.git"
def gitBranch = "master"
def packageName = "sbf"
def projectPrefix = "neueda-capopen"
def newFixVersion = "0.0.1"

def gitCreds = '51947d2c-c215-4578-8996-605330e83e39'
def nodeLabels = ['centos6', 'centos7', 'ubuntu', 'fedora']
def buildTest = [:]

for (int i = 0; i < nodeLabels.size(); i++)
{
    int index = i
    def nodeName = nodeLabels[index]
    stage ("Build-Test")
    {
        buildTest["build-test-$nodeName"] = {
            node (nodeName)
            {
                stage("checkout [$nodeName]") 
                {
                    checkout([$class: 'GitSCM', branches: [[name: "*/$gitBranch"]],
                             doGenerateSubmoduleConfigurations: false,
                             extensions: [[$class: 'SubmoduleOption',
                                     disableSubmodules: false,
                                     parentCredentials: true,
                                     recursiveSubmodules: true,
                                     reference: '',
                                     trackingSubmodules: false]],
                             submoduleCfg: [], userRemoteConfigs: [[credentialsId: gitCreds, url: gitUrl]
                             ]])
                }
                stage("build [$nodeName]")
                {
                    sh("""
                        rm -rf build
                        mkdir build
                        cd build
                        cmake -DTESTS=ON ../
                        make VERBOSE=1 install |& tee compiler.log
                        make package
                    """)
                }
                stage("archive-build")
                {
                    //archive
                    archive "build/*.tar.gz"

                    // find fix-version
                    sh "ls build/$packageName*.tar.gz | awk -F \\- {'print \$2'} > new_fix_version.txt"
                    newFixVersion = readFile ('new_fix_version.txt').trim ()
                    println ('found-fix-version: ' + newFixVersion)
                }
                stage("compiler-warnings")
                {
                    warnings canComputeNew: false, canResolveRelativePaths: false, categoriesPattern: '', consoleParsers: [[parserName: 'GNU C Compiler 4 (gcc)']], defaultEncoding: '', excludePattern: '', healthy: '', includePattern: '', messagesPattern: '', unHealthy: ''
                }
                /* stage("unittest [$nodeName]")
                {
                    sh("""
                       cd build
                       export LD_LIBRARY_PATH=`pwd`/install/lib
                       ./install/bin/unittest --gtest_output=xml:../test.xml
                    """)

                    step([$class: 'XUnitPublisher',
                         testTimeMargin: '3000',
                         thresholdMode: 1,
                         thresholds: [[$class: 'FailedThreshold',
                                 failureNewThreshold: '',
                                 failureThreshold: '',
                                 unstableNewThreshold: '',
                                 unstableThreshold: ''],
                                      [$class: 'SkippedThreshold',
                                      failureNewThreshold: '',
                                      failureThreshold: '',
                                      unstableNewThreshold: '',
                                      unstableThreshold: '']],
                         tools: [[$class: 'GoogleTestType',
                                 deleteOutputFiles: true,
                                 failIfNotNew: true,
                                 pattern: 'test.xml',
                                 skipNoTestFiles: false,
                                 stopProcessingIfError: true]]])
                } */

                stage("static-analysis")
                {
                    sh "cppcheck -f --enable=all --suppress=missingIncludeSystem --xml ./ 2> ./cppcheck_report.xml"
                    sh 'vera++ -p vera-profile -c vera.xml `find ./common ./core ./handlers ./tools ./transport -regextype posix-egrep -regex ".*\\.(cc|cpp|h|hpp)"`'

                    step([$class: 'CheckStylePublisher',
                          pattern: 'vera.xml'])
                }
                stage("sonar-upload")
                {
                    withSonarQubeEnv('SonarQube Server') {
                        sh "/opt/sonar-scanner-2.8/bin/sonar-scanner -Dsonar.cxx.cppcheck.reportPath=cppcheck_report.xml -Dsonar.cxx.vera.reportPath=vera.xml -Dsonar.cxx.compiler.parser=GCC -Dsonar.cxx.compiler.reportPath=build/compiler.log -Dsonar.cxx.compiler.charset=UTF-8 -Dsonar.projectName=$packageName -Dsonar.projectVersion=$newFixVersion -Dsonar.projectKey=$projectPrefix-$nodeName-$packageName -Dsonar.sources=./"
                    }
                }
            }
        }
    }
}

parallel buildTest
