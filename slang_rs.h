/*
 * Copyright 2010, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _SLANG_COMPILER_SLANG_RS_HPP
#define _SLANG_COMPILER_SLANG_RS_HPP

#include "slang.h"

#include <list>
#include <vector>
#include <string>

#include "llvm/ADT/StringMap.h"

#include "slang_rs_reflect_utils.h"

namespace slang {
  class RSContext;
  class RSExportRecordType;

class SlangRS : public Slang {
 private:
  // Context for RenderScript
  RSContext *mRSContext;

  bool mAllowRSPrefix;

  // Custom diagnostic identifiers
  unsigned mDiagErrorInvalidOutputDepParameter;
  unsigned mDiagErrorODR;

  // FIXME: Should be std::list<RSExportable *> here. But currently we only
  //        check ODR on record type.
  //
  // ReflectedDefinitions maps record type name to a pair:
  //  <its RSExportRecordType instance,
  //   the first file contains this record type definition>
  typedef std::pair<RSExportRecordType*, const char*> ReflectedDefinitionTy;
  typedef llvm::StringMap<ReflectedDefinitionTy> ReflectedDefinitionListTy;
  ReflectedDefinitionListTy ReflectedDefinitions;

  // The package name that's really applied will be filled in RealPackageName.
  bool reflectToJava(const std::string &OutputPathBase,
                     const std::string &OutputPackageName,
                     std::string *RealPackageName);

  bool generateBitcodeAccessor(const std::string &OutputPathBase,
                               const std::string &PackageName);

  bool checkODR();

 protected:
  virtual void initDiagnostic();
  virtual void initPreprocessor();
  virtual void initASTContext();

  virtual clang::ASTConsumer
  *createBackend(const clang::CodeGenOptions& CodeGenOpts,
                 llvm::raw_ostream *OS,
                 Slang::OutputType OT);


 public:
  static bool IsRSHeaderFile(const char *File);

  SlangRS();

  // Compile bunch of RS files given in the llvm-rs-cc arguments. Return true if
  // all given input files are successfully compiled without errors.
  //
  // @IOFiles - List of pairs of <input file path, output file path>.
  //
  // @DepFiles - List of pairs of <output dep. file path, dependent bitcode
  //             target>. If @OutputDep is true, this parameter must be given
  //             with the same number of pairs given in @IOFiles.
  //
  // @IncludePaths - User-defined include paths.
  //
  // @AdditionalDepTargets - User-defined files added to the dependencies.
  //
  // @OutputType - See Slang::OutputType.
  //
  // @BitcodeStorage - See BitCodeStorageType in slang_rs_reflect_util.cpp.
  //
  // @AllowRSPrefix - true to allow user-defined function prefixed with 'rs'.
  //
  // @OutputDep - true if output dependecies file for each input file.
  //
  // @JavaReflectionPathBase - The path base for storing reflection files.
  //
  // @JavaReflectionPackageName - The package name given by user in command
  //                              line. This may override the package name
  //                              specified in the .rs using #pragma.
  //
  bool compile(const std::list<std::pair<const char*, const char*> > &IOFiles,
               const std::list<std::pair<const char*, const char*> > &DepFiles,
               const std::vector<std::string> &IncludePaths,
               const std::vector<std::string> &AdditionalDepTargets,
               Slang::OutputType OutputType, BitCodeStorageType BitcodeStorage,
               bool AllowRSPrefix, bool OutputDep,
               const std::string &JavaReflectionPathBase,
               const std::string &JavaReflectionPackageName);

  virtual void reset();

  virtual ~SlangRS();
};
}

#endif  // _SLANG_COMPILER_SLANG_RS_HPP
