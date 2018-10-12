#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <limits>
#include "GetBDoseInfo.h"
#include "BDoseMiniReader.h"
#include "VCFMiniReader.h"
#include "BDoseWriter.h"
#include <Rcpp.h>

// [[Rcpp::export]]
Rcpp::List BDConvertC(const Rcpp::List &bdInfo, const std::string &newFile, const std::string &famFile,
                      const std::string &mapFile, int newFormat, int newVersion) {
  Rcpp::List retVal;
  std::string filetype = bdInfo["filetype"];
  std::string filename = bdInfo["filename"];
  int format = bdInfo["format"];
  int version = bdInfo["version"];
  int numSub = bdInfo["NumSamples"];
  std::vector<int> groupSizes = Rcpp::as<std::vector<int> >(bdInfo["GroupSizes"]);
  Rcpp::DataFrame samples = Rcpp::as<Rcpp::DataFrame>(bdInfo["Samples"]);
  std::vector<std::string> fid = Rcpp::as<std::vector<std::string> >(samples["FID"]);
  std::vector<std::string> sid = Rcpp::as<std::vector<std::string> >(samples["SID"]);
  int numSNPs = bdInfo["NumSNPs"];
  Rcpp::DataFrame snps = Rcpp::as<Rcpp::DataFrame>(bdInfo["SNPs"]);
  std::vector<std::string> snpID = Rcpp::as<std::vector<std::string> >(snps["SNPID"]);
  std::vector<std::string> chromosome = Rcpp::as<std::vector<std::string> >(snps["Chromosome"]);
  std::vector<std::string> ref = Rcpp::as<std::vector<std::string> >(snps["Reference"]);
  std::vector<std::string> alt = Rcpp::as<std::vector<std::string> >(snps["Alternate"]);
  std::vector<int> loc = Rcpp::as<std::vector<int> >(snps["Location"]);
  Rcpp::DataFrame snpInfo = Rcpp::as<Rcpp::DataFrame>(bdInfo["SNPInfo"]);
  std::vector<std::vector<double> > aaf, maf, avgCall, rSq;
  std::vector<double> dblVec;
  std::vector<int>::iterator intIt;
  CBDoseMiniReader *bdmr = NULL;
  CBDoseWriter *bdw;

  if (format == newFormat && newVersion == version) {
    Rcpp::Rcerr << "Old format and new format are the same" << std::endl;
    return retVal;
  }
  if (format < 4)
    bdmr = new CBDoseMiniReader1(filename, numSub, numSNPs);
  else
    bdmr = new CBDoseMiniReader4(filename);

  if (!bdmr->good()) {
    Rcpp::Rcerr << "Error opening binary dosage file" << std::endl;
    delete bdmr;
    return retVal;
  }

  if (newFormat < 4)
    bdw = new CBDoseWriter1(newFile, famFile, mapFile, newFormat, newVersion);
  else
    bdw = new CBDoseWriter4(newFile, newFormat, newVersion);

  if (!bdw->good()) {
    Rcpp::Rcerr << "Unable to open output file" << std::endl;
    delete bdmr;
    delete bdw;
    return retVal;
  }

  if (newFormat == 4 && format == 4) {
    dblVec = Rcpp::as<std::vector<double> >(snpInfo["AAF"]);
    if (dblVec[0] == dblVec[0]) {
      aaf.resize(1);
      aaf[0] = dblVec;
    }
    dblVec = Rcpp::as<std::vector<double> >(snpInfo["MAF"]);
    if (dblVec[0] == dblVec[0]) {
      maf.resize(1);
      maf[0] = dblVec;
    }
    dblVec = Rcpp::as<std::vector<double> >(snpInfo["AvgCall"]);
    if (dblVec[0] == dblVec[0]) {
      avgCall.resize(1);
      avgCall[0] = dblVec;
    }
    dblVec = Rcpp::as<std::vector<double> >(snpInfo["RSquared"]);
    if (dblVec[0] == dblVec[0]) {
      rSq.resize(1);
      rSq[0] = dblVec;
    }
  }

  do {
    if (bdw->WriteGroupData(groupSizes)) {
      Rcpp::Rcerr << "Error writing group data" << std::endl;
      break;
    }
    if (bdw->WriteSubjectData(fid, sid)) {
      Rcpp::Rcerr << "Error writing subject data" << std::endl;
      break;
    }
    if (bdw->WriteSNPData(chromosome, snpID, loc, ref, alt, aaf, maf, avgCall, rSq)) {
      Rcpp::Rcerr << "Error writing SNP data" << std::endl;
      break;
    }

    for (intIt = loc.begin(); intIt != loc.end(); ++intIt) {
      if (intIt == loc.begin())
        bdmr->GetFirst();
      else
        bdmr->GetNext();
      if (bdw->WriteGeneticData(bdmr->Dosage(), bdmr->P0(), bdmr->P1(), bdmr->P2())) {
        Rcpp::Rcerr << "Error genetic data" << std::endl;
        break;
      }
    }
    if (intIt != loc.end())
      break;
  } while (0);
  if (!bdw->good()) {
    Rcpp::Rcerr << "Error writing file" << std::endl;
    delete bdmr;
    delete bdw;
    return retVal;
  }
  if (bdw->Finalize()) {
    Rcpp::Rcerr << "Error finalizing file" << std::endl;
    delete bdmr;
    delete bdw;
    return retVal;
  }

  retVal = Rcpp::List::create(Rcpp::Named("SID") = sid,
                              Rcpp::Named("SNPs") = snpID,
                              Rcpp::Named("Dosage") = bdmr->Dosage());
  if (bdmr)
    delete bdmr;
  if (bdw)
    delete bdw;
  return retVal;
}

// [[Rcpp::export]]
Rcpp::List BDConvertVCFC(const Rcpp::List &vcfInfo, const std::string &newFile, const std::string &famFile,
                         const std::string &mapFile, int newFormat, int newVersion) {
  Rcpp::List retVal;
  std::string filetype = vcfInfo["filetype"];
  std::string filename = vcfInfo["filename"];
  int numSub = vcfInfo["NumSamples"];
  Rcpp::DataFrame samples = Rcpp::as<Rcpp::DataFrame>(vcfInfo["Samples"]);
  std::vector<std::string> fid = Rcpp::as<std::vector<std::string> >(samples["FID"]);
  std::vector<std::string> sid = Rcpp::as<std::vector<std::string> >(samples["SID"]);
  int numSNPs = vcfInfo["NumSNPs"];
  Rcpp::DataFrame snps = Rcpp::as<Rcpp::DataFrame>(vcfInfo["SNPs"]);
  std::vector<std::string> snpID = Rcpp::as<std::vector<std::string> >(snps["SNPID"]);
  std::vector<std::string> chromosome = Rcpp::as<std::vector<std::string> >(snps["Chromosome"]);
  std::vector<std::string> ref = Rcpp::as<std::vector<std::string> >(snps["Reference"]);
  std::vector<std::string> alt = Rcpp::as<std::vector<std::string> >(snps["Alternate"]);
  std::vector<int> loc = Rcpp::as<std::vector<int> >(snps["Location"]);
  int startDataI = vcfInfo["StartData"];
  std::streampos startData = startDataI;
  std::vector<std::vector<double> > aaf, maf, avgCall, rSq;
  std::vector<double> dblVec;
  std::vector<int>::iterator intIt;
  CVCFMiniReader *vcfmr = NULL;
  CBDoseWriter *bdw = NULL;
  std::vector<int> groupSizes;

  vcfmr = new CVCFMiniReader(filename, numSub, numSNPs, startData);
  if (!vcfmr->good()) {
    Rcpp::Rcerr << "Error opening vcf file" << std::endl;
    delete vcfmr;
    return retVal;
  }

  if (newFormat < 4)
    bdw = new CBDoseWriter1(newFile, famFile, mapFile, newFormat, newVersion);
  else
    bdw = new CBDoseWriter4(newFile, newFormat, newVersion);

  if (!bdw->good()) {
    Rcpp::Rcerr << "Unable to open output file" << std::endl;
    delete vcfmr;
    delete bdw;
    return retVal;
  }

  do {
    groupSizes.push_back(numSub);
    if (bdw->WriteGroupData(groupSizes)) {
      Rcpp::Rcerr << "Error writing group data" << std::endl;
      break;
    }
    if (bdw->WriteSubjectData(fid, sid)) {
      Rcpp::Rcerr << "Error writing subject data" << std::endl;
      break;
    }
    if (bdw->WriteSNPData(chromosome, snpID, loc, ref, alt, aaf, maf, avgCall, rSq)) {
      Rcpp::Rcerr << "Error writing SNP data" << std::endl;
      break;
    }

    for (intIt = loc.begin(); intIt != loc.end(); ++intIt) {
      if (intIt == loc.begin())
        vcfmr->GetFirst();
      else
        vcfmr->GetNext();
      if (bdw->WriteGeneticData(vcfmr->Dosage(), vcfmr->P0(), vcfmr->P1(), vcfmr->P2())) {
        Rcpp::Rcerr << "Error genetic data" << std::endl;
        break;
      }
    }
    if (intIt != loc.end())
      break;
  } while (0);
  if (!bdw->good()) {
    Rcpp::Rcerr << "Error writing file" << std::endl;
    delete vcfmr;
    delete bdw;
    return retVal;
  }
  if (bdw->Finalize()) {
    Rcpp::Rcerr << "Error finalizing file" << std::endl;
    delete vcfmr;
    delete bdw;
    return retVal;
  }

  retVal = Rcpp::List::create(Rcpp::Named("SID") = sid,
                              Rcpp::Named("SNPs") = snpID,
                              Rcpp::Named("Dosage") = vcfmr->Dosage());
  if (vcfmr)
    delete vcfmr;
  if (bdw)
    delete bdw;
  return retVal;
}