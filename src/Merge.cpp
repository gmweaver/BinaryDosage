// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; indent-tabs-mode: nil; -*-

// we only include RcppArmadillo.h which pulls Rcpp.h in for us
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <Rcpp.h>
#include "GeneticData.h"
#include "BinaryDosage.h"
#include "WriteBinaryDosage.h"
#include "ReadBinaryDosage.h"
#include "BinaryDosageReader.h"
#include "BinaryDosageWriter.h"

void WriteMergeHeader(std::ofstream &outfile, int numSubjects, int numSNPs, int numGroups) {
  char header[8] = { 'b', 'o', 's', 'e', 0x0, 0x4, 0x0, 0x2 };
  const int zeroInt = 0x0;

  outfile.write(header, 8);
  outfile.write((char *)&numSubjects, sizeof(int));
  outfile.write((char *)&numSNPs, sizeof(int));
  outfile.write((char *)&numGroups, sizeof(int));
  outfile.write((char *)&zeroInt, sizeof(int));
  outfile.write((char *)&zeroInt, sizeof(int));
  outfile.write((char *)&zeroInt, sizeof(int));
  outfile.write((char *)&zeroInt, sizeof(int));
  outfile.write((char *)&zeroInt, sizeof(int));
  outfile.write((char *)&numSubjects, sizeof(int));
}

void WriteMergeSubjects(std::ofstream &outfile, const Rcpp::List &bdList) {
  unsigned int ui, uj;
  std::vector<std::string> subID;

//  for (ui = 0; ui < bdList.size(); ++ui) {
//    subID = Rcpp::as<std::vector<std::string> >(Rcpp::as<Rcpp::List>(bdList[ui])[""])
//    for (uj = 0; uj < Rcpp::as<Rcpp::List>(bdList[ui]).size(); ++uj)
//      outfile.write()
//  }

}

CGeneticData *OpenBinaryDosageFile(const Rcpp::List &geneticInfo) {
  CGeneticData *geneticData = NULL;
  int format, subversion, numSubjects, numSNPs;
  std::string filename;

  format = (int)geneticInfo["format"];
  subversion = (int)geneticInfo["version"];
  numSubjects = (int)geneticInfo["numSubjects"];
  numSNPs = (int)geneticInfo["numSNPs"];
  filename = Rcpp::as<std::string>(geneticInfo["filename"]);

  if (format == 1 && subversion == 1)
    geneticData = new CBinaryDosageFormat1_1(filename, numSubjects, numSNPs);
  else if (format == 1 && subversion == 2)
    geneticData = new CBinaryDosageFormat1_2(filename, numSubjects, numSNPs);
  else if (format == 2 && subversion == 1)
    geneticData = new CBinaryDosageFormat2_1(filename, numSubjects, numSNPs);
  else if (format == 2 && subversion == 2)
    geneticData = new CBinaryDosageFormat2_2(filename, numSubjects, numSNPs);
  else if (format == 3 && subversion == 1)
    geneticData = new CBinaryDosageFormat3_1(filename, numSubjects, numSNPs);
  else if (format == 3 && subversion == 2)
    geneticData = new CBinaryDosageFormat3_2(filename, numSubjects, numSNPs);
  else if (format == 4 && subversion == 1)
    geneticData = new CBinaryDosageFormat4_1(filename, numSubjects, numSNPs);
  else if (format == 4 && subversion == 2)
    geneticData = new CBinaryDosageFormat4_2(filename, numSubjects, numSNPs);
  else
    return NULL;

  return geneticData;
}


//' Function to test opening an array of binary dosage files
//'
//' Function to test opening an array of binary dosage files
//'
//' @param bdList
//' List of binary dosage info lists
//' @param mergeFilename
//' Name of file to merge data into
//' @param snpLoc
//' Locations of snps in the various binary dosage files
//' @return
//' 0 Success
//' 1 failure
//' @export
// [[Rcpp::export]]
int OpenBDFiles(const Rcpp::List &bdList, const std::string &mergeFilename, const Rcpp::NumericMatrix &snpLoc) {
  std::vector<CGeneticData *> bdFiles;
  std::vector<double> d, p0, p1, p2;
  int numSubjects;
  std::ofstream outfile;
  int numFiles;
  int i;

  numFiles = bdList.size();
  bdFiles.resize(numFiles);

  numSubjects = 0;
  for (i = 0; i < numFiles; ++i) {
    bdFiles[i] = OpenBinaryDosageFile(Rcpp::as<Rcpp::List>(bdList[i]));
    numSubjects += bdFiles[i]->NumSubjects();
  }

  outfile.open(mergeFilename.c_str(), std::ios_base::out | std::ios_base::binary);
  Rcpp::Rcout << numSubjects << '\t' << snpLoc.nrow() << std::endl;
  WriteMergeHeader(outfile, numSubjects, snpLoc.nrow(), 1);

  outfile.close();
  for (i = 0; i < numFiles; ++i) {
    if (bdFiles[i])
      delete bdFiles[i];
  }

  return 0;
}

//' Function to test getting dosage values
//'
//' Function to test getting dosage values
//'
//' @param bdInfo
//' Binary dosage info list
//' @return
//' List of dosages
//' @export
// [[Rcpp::export]]
Rcpp::NumericVector GetDosages(const Rcpp::List &bdInfo, int snpNumber) {
  CGeneticData *bdFile = NULL;
  Rcpp::NumericVector d((int)bdInfo["numSubjects"]);
  int i;

  bdFile = OpenBinaryDosageFile(bdInfo);
  if (bdFile == NULL)
    return NULL;

  bdFile->GetFirst();
  for (i = 0; i < bdFile->NumSubjects(); ++i)
    d[i] = bdFile->Dosages()[i];

  if (bdFile)
    delete bdFile;

  return d;
}

//' Function to test getting dosage values
//'
//' Function to test getting dosage values
//'
//' @param mergeFilename
//' Name of file containing merged data
//' @param filenames
//' Names of files to merge
//' @param mergeInfo
//' Information on files to merge and intersection of SNPs
//' and union of subjects
//' @return
//' List of dosages
//' @export
// [[Rcpp::export]]
int Merge42C(std::string &mergeFilename, Rcpp::StringVector &filenames, Rcpp::List &mergeInfo) {
  std::vector<CBinaryDosageReader4 *> bdr;
  std::vector<CBinaryDosageReader4 *>::iterator bdrIt;
  CBinaryDosageReader4 *bdrTemp;
  std::vector<std::string> bdFilename = Rcpp::as<std::vector<std::string> >(filenames);
  Rcpp::IntegerMatrix snpsToUse = mergeInfo["locations"];
  Rcpp::DataFrame subjects(mergeInfo["subjects"]);
  std::vector<std::string> fid = Rcpp::as<std::vector<std::string> >(subjects["fid"]);
  std::vector<std::string> iid = Rcpp::as<std::vector<std::string> >(subjects["iid"]);
  Rcpp::List snpsToMerge = mergeInfo["snpsToMerge"];
  Rcpp::DataFrame snps(snpsToMerge["SNPs"]);
  std::vector<std::string> snpID = Rcpp::as<std::vector<std::string> >(snps["SNPID"]);
  std::vector<std::string> chromosome = Rcpp::as<std::vector<std::string> >(snps["Chromosome"]);
  std::vector<int> bp = Rcpp::as<std::vector<int> >(snps["Location"]);
  std::vector<std::string> refAllele = Rcpp::as<std::vector<std::string> >(snps["Reference"]);
  std::vector<std::string> altAllele = Rcpp::as<std::vector<std::string> >(snps["Alternate"]);
  std::vector<std::vector<double> > altAlleleFreq, maf, avgCall, rSq;
  std::vector<std::vector<double> >::iterator vDblIt;
  std::vector<double>::iterator dblIt1, dblIt2, dblIt3, dblIt4;
  std::vector<unsigned int> groups;
  std::vector<double> dosage, p0, p1, p2;
  double af;
  int subNum;
  bool bdrGood;

  altAlleleFreq.resize(chromosome.size());
  for (vDblIt = altAlleleFreq.begin(); vDblIt != altAlleleFreq.end(); ++vDblIt)
    vDblIt->resize(1);
  groups.push_back(iid.size());
  CBinaryDosageWriter4 bdw(mergeFilename, 4, 2, groups, fid, iid,
                           chromosome, snpID, bp, refAllele, altAllele,
                           altAlleleFreq, maf, avgCall, rSq);
  if (bdw.good())
    Rcpp::Rcout << "File successfully created" << std::endl;
  else
    Rcpp::Rcout << "Failed to create file" << std::endl;

  bdr.resize(bdFilename.size());
  bdrIt = bdr.begin();
  bdrGood = true;
  for (std::vector<std::string>::iterator strIt = bdFilename.begin(); strIt != bdFilename.end(); ++strIt, ++bdrIt) {
    Rcpp::Rcout << *strIt << std::endl;
    bdrTemp = new CBinaryDosageReader4(*strIt);
    if (!bdrTemp->good()) {
      bdrGood = false;
      Rcpp::Rcout << "Failed to open binary dosage file " << *strIt << std::endl;
    }
    else
      Rcpp::Rcout << "Successfully opened binary dosage file " << *strIt << std::endl;
    *bdrIt = bdrTemp;
  }
  if (!bdrGood) {
    for (bdrIt = bdr.begin(); bdrIt != bdr.end(); ++bdrIt) {
      if (*bdrIt != NULL)
        delete *bdrIt;
    }
    Rcpp::Rcout << "Not all files opened successfully" << std::endl;
    return 1;
  }

  dosage.resize(subjects.nrow());
  p0.resize(subjects.nrow());
  p1.resize(subjects.nrow());
  p2.resize(subjects.nrow());
  for (int i = 0; i < snpsToUse.nrow() && bdrGood; ++i) {
    dblIt1 = dosage.begin();
    dblIt2 = p0.begin();
    dblIt3 = p1.begin();
    dblIt4 = p2.begin();
    for (unsigned int j = 0; j < snpsToUse.ncol(); ++j) {
      Rcpp::Rcout << "Reading SNP " << snpsToUse(i, j) << " from " << bdFilename[j] << std::endl;
      if (bdr[j]->GetSNP(snpsToUse(i, j)) == false) {
        Rcpp::Rcout << "Error reading SNP " << i << " from " << bdFilename[j] << std::endl;
        bdrGood = false;
        break;
      }
      for (unsigned int uk = 0; uk < bdr[j]->NumSamples(); ++uk, ++dblIt1, ++dblIt2, ++dblIt3, ++dblIt4) {
        *dblIt1 = bdr[j]->Dosage()[uk];
        *dblIt2 = bdr[j]->P0()[uk];
        *dblIt3 = bdr[j]->P1()[uk];
        *dblIt4 = bdr[j]->P2()[uk];
//        Rcpp::Rcout << *dblIt1 << '\t' << *dblIt2 << '\t' << *dblIt3 << '\t' << *dblIt4 << std::endl;
      }
    }
    if (bdrGood) {
      if (bdw.WriteGeneticData(dosage, p0, p1, p2))
        bdrGood = false;
      else {
        for (dblIt1 = dosage.begin(); dblIt1 != dosage.end(); ++dblIt1)
          Rcpp::Rcout << *dblIt1 << '\t';
        Rcpp::Rcout << std::endl;
        altAlleleFreq[i][0] = std::accumulate(dosage.begin(), dosage.end(), 0.) / (subjects.nrow() + subjects.nrow());
      }
    }
  }
  if (bdrGood) {
    Rcpp::Rcout << "File successfully created" << std::endl;
    for (vDblIt = altAlleleFreq.begin(); vDblIt != altAlleleFreq.end(); ++vDblIt)
      Rcpp::Rcout << *(vDblIt->begin()) << '\t';
    Rcpp::Rcout << std::endl;
    bdw.UpdateAlternateAlleleFreq(altAlleleFreq);
  }

  for (bdrIt = bdr.begin(); bdrIt != bdr.end(); ++bdrIt) {
    if (*bdrIt != NULL)
      delete *bdrIt;
  }
/*
  std::vector<std::string> mergeStringVector;
  mergeStringVector.resize(1);
  mergeStringVector[0] = mergeFilename;
  CWriteBinaryDosage42 bdOut(mergeStringVector);

  bdOut.WriteHeader();
  groups.resize(1);
  groups[0] = subjects.nrow();
  bdOut.WriteGroups(groups);

  if (fid[0] == "") {
    fid.resize(0);
  }
  bdOut.WriteSubjects(fid, iid);

  altAlleleFreq.resize(chromosome.size());
  for (std::vector<std::vector<double> >::iterator vdit = altAlleleFreq.begin(); vdit != altAlleleFreq.end(); ++vdit)
    vdit->resize(1);
  if (bdOut.WriteAllSNPs(chromosome, snpID, bp, refAllele, altAllele, altAlleleFreq, maf, avgCall, rSq))
    Rcpp::Rcout << "Error writing SNP data" << std::endl;

  bdFilename.resize(1);
  bdFiles.resize(filenames.length());
  for (int i = 0; i < filenames.length(); ++i) {
    bdFilename[0] = filenames(i);
    bdf42 = new CReadBinaryDosage42(bdFilename);
    bdf42->ReadHeader();
    bdf42->ReadSubjects();
    bdf42->ReadGroups();
    bdf42->ReadSNPs();
    bdf42->GetFirst();
//    bdf42->WriteData(Rcpp::Rcout);
    bdFiles[i] = bdf42;
  }

  geneticValues.resize(4);
  geneticValues[0].resize(subjects.nrow());
  geneticValues[1].resize(subjects.nrow());
  geneticValues[2].resize(subjects.nrow());
  geneticValues[3].resize(subjects.nrow());

  for (int i = 0; i < snpsToUse.nrow(); ++i) {
    subNum = 0;
    for (unsigned int j = 0; j < snpsToUse.ncol(); ++j) {
      if (bdFiles[j]->GetSNP(snpsToUse(i, j) - 1))
        Rcpp::Rcout << "Error reading SNP from file number" << j << std::endl;
      for (unsigned int uk = 0; uk < bdFiles[j]->NumSubjects(); ++uk, ++subNum) {
        geneticValues[0][subNum] = bdFiles[j]->Dosage()[uk];
        geneticValues[1][subNum] = bdFiles[j]->P0()[uk];
        geneticValues[2][subNum] = bdFiles[j]->P1()[uk];
        geneticValues[3][subNum] = bdFiles[j]->P2()[uk];
      }
    }
    af = std::accumulate(geneticValues[0].begin(), geneticValues[0].end(), 0.) / (subNum + subNum);
    Rcpp::Rcout << af << std::endl;
    altAlleleFreq[i][0] = af;
    if (bdOut.AddGeneticValues(geneticValues))
      Rcpp::Rcout << "Error adding genetic values" << std::endl;
  }
  if (bdOut.WriteAllSNPs(chromosome, snpID, bp, refAllele, altAllele, altAlleleFreq, maf, avgCall, rSq))
    Rcpp::Rcout << "Error writing SNP data" << std::endl;

  for (int i = 0; i < bdFiles.size(); ++i) {
    if (bdFiles[i])
      delete bdFiles[i];
  }
   */
  return 0;
}
