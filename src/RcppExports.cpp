// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// GetBinaryDosageInfoC
Rcpp::List GetBinaryDosageInfoC(const std::string& bdFilename, const std::string& famFilename, const std::string& mapFilename, const int index);
RcppExport SEXP _BinaryDosage_GetBinaryDosageInfoC(SEXP bdFilenameSEXP, SEXP famFilenameSEXP, SEXP mapFilenameSEXP, SEXP indexSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::string& >::type bdFilename(bdFilenameSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type famFilename(famFilenameSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type mapFilename(mapFilenameSEXP);
    Rcpp::traits::input_parameter< const int >::type index(indexSEXP);
    rcpp_result_gen = Rcpp::wrap(GetBinaryDosageInfoC(bdFilename, famFilename, mapFilename, index));
    return rcpp_result_gen;
END_RCPP
}
// GetSNPValuesC
int GetSNPValuesC(const std::string& filename, const std::string& filetype, int geneProb, const Rcpp::IntegerVector& subVec, const Rcpp::IntegerVector snpVec, const Rcpp::IntegerVector& indices, Rcpp::NumericMatrix& valueMatrix, const int numSubjects, const int numSNPs);
RcppExport SEXP _BinaryDosage_GetSNPValuesC(SEXP filenameSEXP, SEXP filetypeSEXP, SEXP geneProbSEXP, SEXP subVecSEXP, SEXP snpVecSEXP, SEXP indicesSEXP, SEXP valueMatrixSEXP, SEXP numSubjectsSEXP, SEXP numSNPsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::string& >::type filename(filenameSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type filetype(filetypeSEXP);
    Rcpp::traits::input_parameter< int >::type geneProb(geneProbSEXP);
    Rcpp::traits::input_parameter< const Rcpp::IntegerVector& >::type subVec(subVecSEXP);
    Rcpp::traits::input_parameter< const Rcpp::IntegerVector >::type snpVec(snpVecSEXP);
    Rcpp::traits::input_parameter< const Rcpp::IntegerVector& >::type indices(indicesSEXP);
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix& >::type valueMatrix(valueMatrixSEXP);
    Rcpp::traits::input_parameter< const int >::type numSubjects(numSubjectsSEXP);
    Rcpp::traits::input_parameter< const int >::type numSNPs(numSNPsSEXP);
    rcpp_result_gen = Rcpp::wrap(GetSNPValuesC(filename, filetype, geneProb, subVec, snpVec, indices, valueMatrix, numSubjects, numSNPs));
    return rcpp_result_gen;
END_RCPP
}
// GetVCFHeaderC
Rcpp::List GetVCFHeaderC(std::string& vcfFile);
RcppExport SEXP _BinaryDosage_GetVCFHeaderC(SEXP vcfFileSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string& >::type vcfFile(vcfFileSEXP);
    rcpp_result_gen = Rcpp::wrap(GetVCFHeaderC(vcfFile));
    return rcpp_result_gen;
END_RCPP
}
// GetVCFSNPInfoC
Rcpp::List GetVCFSNPInfoC(std::string& filename, int startData, int reserve);
RcppExport SEXP _BinaryDosage_GetVCFSNPInfoC(SEXP filenameSEXP, SEXP startDataSEXP, SEXP reserveSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string& >::type filename(filenameSEXP);
    Rcpp::traits::input_parameter< int >::type startData(startDataSEXP);
    Rcpp::traits::input_parameter< int >::type reserve(reserveSEXP);
    rcpp_result_gen = Rcpp::wrap(GetVCFSNPInfoC(filename, startData, reserve));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_BinaryDosage_GetBinaryDosageInfoC", (DL_FUNC) &_BinaryDosage_GetBinaryDosageInfoC, 4},
    {"_BinaryDosage_GetSNPValuesC", (DL_FUNC) &_BinaryDosage_GetSNPValuesC, 9},
    {"_BinaryDosage_GetVCFHeaderC", (DL_FUNC) &_BinaryDosage_GetVCFHeaderC, 1},
    {"_BinaryDosage_GetVCFSNPInfoC", (DL_FUNC) &_BinaryDosage_GetVCFSNPInfoC, 3},
    {NULL, NULL, 0}
};

RcppExport void R_init_BinaryDosage(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
