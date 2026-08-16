#pragma once

#include "TPaveText.h"
#include "aux/uti.h"
#include "RooWorkspace.h"
#include "TString.h"

using namespace RooFit;
using namespace std;

Int_t _count=0;
RooFitResult *fit(TString system, TString variation, TString pdf, TString tree, TCanvas* c, TCanvas* cMC, RooDataSet* ds, RooDataSet* dsMC, RooRealVar* mass, float binmin, float binmax, RooWorkspace& w, TString which_var, int NBIN){
	double init_mean = Bs_MASS;
	if (tree == "ntKp") init_mean = Bu_MASS;
	else if (tree == "ntKstar") init_mean = Bd_MASS;
	else if (tree == "ntmix_X3872") init_mean = X3872_MASS;
	else if (tree == "ntmix_PSI2S") init_mean = PSI2S_MASS;
	
	double init_sigma1 = 0.01, init_sigma2 = 0.005, min_sigma1 = 0.001, max_sigma1 = 0.1, min_sigma2 = 0.001, max_sigma2 = 0.1;
	if (system == "ppRef_nonPrompt") { init_sigma1 = 0.01; init_sigma2 = 0.005; min_sigma1 = 0.005; max_sigma1 = 0.1; min_sigma2 = 0.004; max_sigma2 = 0.01; }	

	RooRealVar mean(Form("mean%d_%s", _count, pdf.Data()), "", init_mean, init_mean - 0.01, init_mean + 0.01);
	RooRealVar sigma1(Form("sigma1%d_%s", _count, pdf.Data()), "", init_sigma1, min_sigma1, max_sigma1);
	RooRealVar sigma2(Form("sigma2%d_%s", _count, pdf.Data()), "", init_sigma2, min_sigma2, max_sigma2);
	RooRealVar sigma3(Form("sigma3%d_%s", _count, pdf.Data()), "", 0.01, 0.001, 0.03);
	RooRealVar sigma4cb(Form("sigma4cb%d_%s", _count, pdf.Data()), "", 0.005, 0.001, 0.05);
	RooRealVar alpha(Form("alpha%d_%s", _count, pdf.Data()), "", 4., 0, 15);
	RooRealVar n(Form("n_%d_%s", _count, pdf.Data()), "", 10, -100, 200);
	RooRealVar* scale = new RooRealVar("scale", "scale", 1, 0.5, 1.5);
	RooProduct scaled_sigma1(Form("scaled_sigma1%d_%s", _count, pdf.Data()), "scaled_sigma1", RooArgList(*scale, sigma1));
	RooProduct scaled_sigma2(Form("scaled_sigma2%d_%s", _count, pdf.Data()), "scaled_sigma2", RooArgList(*scale, sigma2));
	RooProduct scaled_sigma3(Form("scaled_sigma3%d_%s", _count, pdf.Data()), "scaled_sigma3", RooArgList(*scale, sigma3));
	RooProduct scaled_sigmacb(Form("scaled_sigmacb%d_%s", _count, pdf.Data()), "scaled_sigmacb", RooArgList(*scale, sigma4cb));
	RooGaussian sig1(Form("sig1%d_%s", _count, pdf.Data()), "", *mass, mean, scaled_sigma1);
	RooGaussian sig2(Form("sig2%d_%s", _count, pdf.Data()), "", *mass, mean, scaled_sigma2);
	RooGaussian sig3(Form("sig3%d_%s", _count, pdf.Data()), "", *mass, mean, scaled_sigma3);
	RooCBShape CB(Form("CB%d_%s", _count, pdf.Data()), "", *mass, mean, scaled_sigmacb, alpha, n);
	RooRealVar sig1frac(Form("sig1frac%d_%s", _count, pdf.Data()), "", 0.5, 0.01, 1);
	RooRealVar sig2frac(Form("sig2frac%d_%s", _count, pdf.Data()), "", 0.5, 0.01, 1);

	// ============================================================
	// Signal and MC model pointers
	// ============================================================

	RooAddPdf* sig = nullptr;
	RooAbsPdf* modelMC = nullptr;
	RooFitResult* fitResultMC = nullptr;
	RooRealVar* nsigMC = nullptr;


	// Objects needed only for B0 RTWT
	RooDataSet* dsMC_RT = nullptr;
	RooDataSet* dsMC_WT = nullptr;
	RooAddPdf* shapeRT = nullptr;
	RooAddPdf* shapeWT = nullptr;
	RooAddPdf* totalMC = nullptr;

	RooRealVar* meanRT = nullptr;
	RooRealVar* meanWT = nullptr;

	RooRealVar* sigma1RT = nullptr;
	RooRealVar* sigma2RT = nullptr;
	RooRealVar* sigma3RT = nullptr;
	RooRealVar* frac1RT = nullptr;
	RooRealVar* frac2RT = nullptr;


	RooRealVar* sigmaGWT = nullptr;
	RooRealVar* sigmaLWT = nullptr;
	RooRealVar* sigmaRWT = nullptr;
	RooRealVar* fracWTG = nullptr;

	RooRealVar* nRT = nullptr;
	RooRealVar* nWT = nullptr;
	RooRealVar* fWTMC = nullptr;

	
	/*if (tree == "ntKstar") {
		dsMC_WT = static_cast<RooDataSet*>(dsMC->reduce("Bgen==41000"));
		dsMC_RT = static_cast<RooDataSet*>(dsMC->reduce("((Bgen == 23333) || (Bgen == 24333) || (Bgen == 23433) || (Bgen == 24433))"));
		std::cout << "Bd RT candidates: " << dsMC_RT->sumEntries() << std::endl;
		std::cout << "Bd WT candidates: " << dsMC_WT->sumEntries() << std::endl;
	}*/


	/*if ((variation == "" && pdf == "") || variation == "background" || (variation == "signal" && pdf == "fixed")) sig = new RooAddPdf(Form("sig_doubleG%d_%s", _count, pdf.Data()), "", RooArgList(sig1, sig2), sig1frac);
	if (variation == "signal" && pdf == "1gauss") sig = new RooAddPdf(Form("sig_Gaussian%d_%s", _count, pdf.Data()), "", RooArgList(sig1), RooArgList(), true);
	if (variation == "signal" && pdf == "3gauss") sig = new RooAddPdf(Form("sig_tripleG%d_%s", _count, pdf.Data()), "", RooArgList(sig1, sig2, sig3), RooArgList(sig1frac, sig2frac), true);
	if (variation == "signal" && pdf == "gauss_cb") sig = new RooAddPdf(Form("sig_gaussCB%d_%s", _count, pdf.Data()), "", RooArgList(sig1, CB), sig1frac);*/

	//RooRealVar* nsigMC = new RooRealVar(Form("nsigMC%d_%s", _count, pdf.Data()), "", dsMC->sumEntries(), 0.9 * dsMC->sumEntries(), 1.1 * dsMC->sumEntries());
	//RooAddPdf* modelMC = new RooAddPdf(Form("modelMC%d_%s", _count, pdf.Data()), "", RooArgList(*sig), RooArgList(*nsigMC));
	//scale->setConstant();

	mass->setRange("signal", init_mean - 0.07, init_mean + 0.07);
	if (tree == "ntKp") mass->setRange("signal", init_mean - 0.1, init_mean + 0.1);
	else if (tree == "ntKstar") mass->setRange("signal", init_mean - 0.2, init_mean + 0.2);
	else if (tree == "ntphi") mass->setRange("signal", init_mean - 0.07, init_mean + 0.07);
	else if (tree == "ntmix_X3872") mass->setRange("signal", init_mean - 0.035, init_mean + 0.035);
	else if (tree == "ntmix_PSI2S") mass->setRange("signal", init_mean - 0.03, init_mean + 0.03);
    
	// ============================================================
	// Special Bd RT/WT MC treatment
	// ============================================================

	if (tree == "ntKstar") {

		std::cout << "\n========================================" << std::endl;
		std::cout << "Running Bd RT/WT MC fit" << std::endl;
		std::cout << "========================================\n" << std::endl;

		// Check that Bgen exists inside dsMC
		if (!dsMC->get()->find("Bgen")) {
			std::cerr
				<< "ERROR: Bgen is not present in the MC RooDataSet."
				<< std::endl;
			std::cerr
				<< "Add Bgen to ANA_vars_mc in roofitB.C."
				<< std::endl;

			return nullptr;
		}

		// Replace YOUR_RT_CODE with the actual right-tag Bgen code
		dsMC_RT = static_cast<RooDataSet*>(
			dsMC->reduce("Bgen != 41000")
		);

		dsMC_WT = static_cast<RooDataSet*>(
			dsMC->reduce("Bgen==41000")
		);

		if (!dsMC_RT || !dsMC_WT ||
			dsMC_RT->numEntries() == 0 ||
			dsMC_WT->numEntries() == 0) {

			std::cerr
				<< "ERROR: Empty Bd RT or WT dataset."
				<< std::endl;

			if (dsMC_RT) {
				std::cerr
					<< "RT entries = "
					<< dsMC_RT->numEntries()
					<< std::endl;
			}

			if (dsMC_WT) {
				std::cerr
					<< "WT entries = "
					<< dsMC_WT->numEntries()
					<< std::endl;
			}

			return nullptr;
		}

		std::cout
			<< "Bd RT entries = "
			<< dsMC_RT->sumEntries()
			<< std::endl;

		std::cout
			<< "Bd WT entries = "
			<< dsMC_WT->sumEntries()
			<< std::endl;

		// ========================================================
		// RT model: double Gaussian
		// ========================================================

		meanRT = new RooRealVar(
			Form("meanRT%d_%s", _count, pdf.Data()),
			"RT mean",
			Bd_MASS,
			Bd_MASS - 0.03,
			Bd_MASS + 0.03
		);

		sigma1RT = new RooRealVar(
			Form("sigma1RT%d_%s", _count, pdf.Data()),
			"RT narrow width",
			0.014,
			0.002,
			0.080
		);

		sigma2RT = new RooRealVar(
			Form("sigma2RT%d_%s", _count, pdf.Data()),
			"RT broad width",
			0.034,
			0.005,
			0.150
		);

		sigma3RT = new RooRealVar(
			Form("sigma3RT%d_%s", _count, pdf.Data()),
			"RT third width",
			0.010,
			0.001,
			0.030
		);

		frac1RT = new RooRealVar(
			Form("frac1RT%d_%s", _count, pdf.Data()),
			"RT narrow fraction",
			0.80,
			0.01,
			0.99
		);

		frac2RT = new RooRealVar(
			Form("frac2RT%d_%s", _count, pdf.Data()),
			"RT broad fraction",
			0.20,
			0.01,
			0.99
		);

		// ========================================================
		// WT model: Gaussian + bifurcated Gaussian
		// ========================================================

		meanWT = new RooRealVar(Form("meanWT%d_%s", _count, pdf.Data()), "WT mean", Bd_MASS, Bd_MASS - 0.10, Bd_MASS + 0.10);
		sigmaGWT = new RooRealVar(Form("sigmaGWT%d_%s", _count, pdf.Data()), "WT Gaussian width", 0.020, 0.002, 0.150);
		sigmaLWT = new RooRealVar(Form("sigmaLWT%d_%s", _count, pdf.Data()), "WT left width", 0.030, 0.002, 0.250);
		sigmaRWT = new RooRealVar(Form("sigmaRWT%d_%s", _count, pdf.Data()), "WT right width", 0.050, 0.002, 0.250);
		fracWTG = new RooRealVar(Form("fracWTG%d_%s", _count, pdf.Data()), "WT Gaussian fraction", 0.50, 0.01, 0.99);
		// Fit the MC widths with scale fixed at one. The data fit releases
		// scale so these products apply a common resolution correction.
		scale->setConstant(true);
		RooProduct* sigma1RT_eff = new RooProduct(Form("scaled_sigma1RT%d_%s", _count, pdf.Data()), "scaled RT narrow width", RooArgList(*scale, *sigma1RT));
		RooProduct* sigma2RT_eff = new RooProduct(Form("scaled_sigma2RT%d_%s", _count, pdf.Data()), "scaled RT broad width", RooArgList(*scale, *sigma2RT));
		RooProduct* sigma3RT_eff = new RooProduct(Form("scaled_sigma3RT%d_%s", _count, pdf.Data()), "scaled RT third width", RooArgList(*scale, *sigma3RT));
		RooProduct* sigmaGWT_eff = new RooProduct(Form("scaled_sigmaGWT%d_%s", _count, pdf.Data()), "scaled WT Gaussian width", RooArgList(*scale, *sigmaGWT));
		RooProduct* sigmaLWT_eff = new RooProduct(Form("scaled_sigmaLWT%d_%s", _count, pdf.Data()), "scaled WT left width", RooArgList(*scale, *sigmaLWT));
		RooProduct* sigmaRWT_eff = new RooProduct(Form("scaled_sigmaRWT%d_%s", _count, pdf.Data()), "scaled WT right width", RooArgList(*scale, *sigmaRWT));

		RooGaussian* g1RT = new RooGaussian(Form("g1RT%d_%s", _count, pdf.Data()), "RT narrow Gaussian", *mass, *meanRT, *sigma1RT_eff);
		RooGaussian* g2RT = new RooGaussian(Form("g2RT%d_%s", _count, pdf.Data()), "RT broad Gaussian", *mass, *meanRT, *sigma2RT_eff);
		RooGaussian* g3RT = new RooGaussian(Form("g3RT%d_%s", _count, pdf.Data()), "RT third Gaussian", *mass, *meanRT, *sigma3RT_eff);
		RooCBShape* cbRT = new RooCBShape(Form("cbRT%d_%s", _count, pdf.Data()), "RT Crystal Ball", *mass, *meanRT, scaled_sigmacb, alpha, n);
		RooGaussian* gWT = new RooGaussian(Form("gWT%d_%s", _count, pdf.Data()), "WT Gaussian", *mass, *meanWT, *sigmaGWT_eff);
		RooBifurGauss* bifWT = new RooBifurGauss(Form("bifWT%d_%s", _count, pdf.Data()), "WT asymmetric Gaussian", *mass, *meanWT, *sigmaLWT_eff, *sigmaRWT_eff);

		if ((variation == "" && pdf == "") || variation == "background" || (variation == "signal" && pdf == "fixed")) shapeRT = new RooAddPdf(Form("sig_doubleG%d_%s", _count, pdf.Data()), "", RooArgList(*g1RT, *g2RT), RooArgList(*frac1RT));
		if (variation == "signal" && pdf == "1gauss") shapeRT = new RooAddPdf(Form("sig_Gaussian%d_%s", _count, pdf.Data()), "", RooArgList(*g1RT), RooArgList(), true);
		if (variation == "signal" && pdf == "3gauss") shapeRT = new RooAddPdf(Form("sig_tripleG%d_%s", _count, pdf.Data()), "", RooArgList(*g1RT, *g2RT, *g3RT), RooArgList(*frac1RT, *frac2RT), true);
		if (variation == "signal" && pdf == "gauss_cb") shapeRT = new RooAddPdf(Form("sig_gaussCB%d_%s", _count, pdf.Data()), "", RooArgList(*g1RT, *cbRT), RooArgList(*frac1RT));

		if (!shapeRT) {
			std::cerr << "ERROR: No Bd RT PDF for variation=" << variation
				<< ", pdf=" << pdf << std::endl;
			return nullptr;
		}

		shapeWT = new RooAddPdf(Form("shapeWT%d_%s", _count, pdf.Data()), "WT Gaussian plus asymmetric Gaussian", RooArgList(*gWT, *bifWT), RooArgList(*fracWTG));

		// ========================================================
		// First MC fit: determine RT and WT shapes simultaneously
		// ========================================================

		RooRealVar* nRTshape = new RooRealVar(
			Form("nRTshape%d_%s", _count, pdf.Data()),
			"RT shape-fit yield",
			dsMC_RT->sumEntries(),
			0.5 * dsMC_RT->sumEntries(),
			1.5 * dsMC_RT->sumEntries()
		);

		RooRealVar* nWTshape = new RooRealVar(
			Form("nWTshape%d_%s", _count, pdf.Data()),
			"WT shape-fit yield",
			dsMC_WT->sumEntries(),
			0.5 * dsMC_WT->sumEntries(),
			1.5 * dsMC_WT->sumEntries()
		);

		RooExtendPdf* modelRTshape = new RooExtendPdf(
			Form("modelRTshape%d_%s", _count, pdf.Data()),
			"RT extended shape model",
			*shapeRT,
			*nRTshape
		);

		RooExtendPdf* modelWTshape = new RooExtendPdf(
			Form("modelWTshape%d_%s", _count, pdf.Data()),
			"WT extended shape model",
			*shapeWT,
			*nWTshape
		);

		RooCategory* sample = new RooCategory(
			Form("sample%d_%s", _count, pdf.Data()),
			"RT WT category"
		);

		sample->defineType("RT");
		sample->defineType("WT");

		RooDataSet* combData = new RooDataSet(
			Form("combData%d_%s", _count, pdf.Data()),
			"Combined RT and WT dataset",
			RooArgSet(*mass),
			Index(*sample),
			Import("RT", *dsMC_RT),
			Import("WT", *dsMC_WT)
		);

		RooSimultaneous* simPdf = new RooSimultaneous(
			Form("simPdf%d_%s", _count, pdf.Data()),
			"Simultaneous RT WT model",
			*sample
		);

		simPdf->addPdf(*modelRTshape, "RT");
		simPdf->addPdf(*modelWTshape, "WT");

		RooFitResult* shapeFitResult = simPdf->fitTo(
			*combData,
			Save(),
			Extended(true),
			Range("signal"),
			Strategy(2),
			Offset(true),
			PrintLevel(-1)
		);

		if (!shapeFitResult) {
			std::cerr
				<< "ERROR: Bd RT/WT shape fit failed."
				<< std::endl;

			return nullptr;
		}

		std::cout
			<< "RT/WT shape fit status = "
			<< shapeFitResult->status()
			<< std::endl;

		std::cout
			<< "RT/WT shape fit covQual = "
			<< shapeFitResult->covQual()
			<< std::endl;

		// Freeze the RT and WT shapes after the truth-separated fit
		meanRT->setConstant(true);
		meanWT->setConstant(true);

		sigma1RT->setConstant(true);
		sigma2RT->setConstant(true);
		sigma3RT->setConstant(true);
		frac1RT->setConstant(true);
		frac2RT->setConstant(true);

		sigmaGWT->setConstant(true);
		sigmaLWT->setConstant(true);
		sigmaRWT->setConstant(true);
		fracWTG->setConstant(true);

		// ========================================================
		// Second MC fit: fit total MC with fixed RT and WT shapes
		// ========================================================

		nRT = new RooRealVar(
			Form("nRT%d_%s", _count, pdf.Data()),
			"Total-MC RT yield",
			dsMC_RT->sumEntries(),
			0.0,
			1.5 * dsMC->sumEntries()
		);

		nWT = new RooRealVar(
			Form("nWT%d_%s", _count, pdf.Data()),
			"Total-MC WT yield",
			dsMC_WT->sumEntries(),
			0.0,
			1.5 * dsMC->sumEntries()
		);

		totalMC = new RooAddPdf(
			Form("totalMC%d_%s", _count, pdf.Data()),
			"Total Bd RT plus WT MC",
			RooArgList(*shapeRT, *shapeWT),
			RooArgList(*nRT, *nWT)
		);

		fitResultMC = totalMC->fitTo(
			*dsMC,
			Save(),
			Extended(true),
			Range("signal"),
			Strategy(2),
			Offset(true),
			PrintLevel(-1)
		);

		if (!fitResultMC) {
			std::cerr
				<< "ERROR: Total Bd MC fit failed."
				<< std::endl;

			return nullptr;
		}

		std::cout
			<< "Total Bd MC fit status = "
			<< fitResultMC->status()
			<< std::endl;

		std::cout
			<< "Total Bd MC fit covQual = "
			<< fitResultMC->covQual()
			<< std::endl;

		std::cout
			<< "Fitted RT yield = "
			<< nRT->getVal()
			<< " +/- "
			<< nRT->getError()
			<< std::endl;

		std::cout
			<< "Fitted WT yield = "
			<< nWT->getVal()
			<< " +/- "
			<< nWT->getError()
			<< std::endl;

		const double totalFittedMC =
			nRT->getVal() + nWT->getVal();

		if (totalFittedMC <= 0.0) {
			std::cerr
				<< "ERROR: Total fitted Bd MC yield is zero."
				<< std::endl;

			return nullptr;
		}

		const double fittedFWT =
			nWT->getVal() / totalFittedMC;

		fWTMC = new RooRealVar(
			Form("fWTMC%d_%s", _count, pdf.Data()),
			"Fitted WT fraction",
			fittedFWT
		);

		fWTMC->setConstant(true);

		// Final normalized signal shape for the DATA fit
		// sig = fWT * WT + (1-fWT) * RT
		sig = new RooAddPdf(
			Form("sigBdRTWT%d_%s", _count, pdf.Data()),
			"Bd RT plus WT signal",
			RooArgList(*shapeWT, *shapeRT),
			RooArgList(*fWTMC)
		);

		// Keep the nsigMC name expected by roofitB.C
		nsigMC = new RooRealVar(
			Form("nsigMC%d_%s", _count, pdf.Data()),
			"Total fitted Bd MC yield",
			totalFittedMC
		);

		nsigMC->setError(
			std::sqrt(
				std::pow(nRT->getError(), 2) +
				std::pow(nWT->getError(), 2)
			)
		);

		nsigMC->setConstant(true);

		modelMC = totalMC;

	}

// ============================================================
// Ordinary MC fit for all non-Bd channels
// ============================================================

	else {

	if ((variation == "" && pdf == "") || variation == "background" || (variation == "signal" && pdf == "fixed")) sig = new RooAddPdf(Form("sig_doubleG%d_%s", _count, pdf.Data()), "", RooArgList(sig1, sig2), sig1frac);
	if (variation == "signal" && pdf == "1gauss") sig = new RooAddPdf(Form("sig_Gaussian%d_%s", _count, pdf.Data()), "", RooArgList(sig1), RooArgList(), true);
	if (variation == "signal" && pdf == "3gauss") sig = new RooAddPdf(Form("sig_tripleG%d_%s", _count, pdf.Data()), "", RooArgList(sig1, sig2, sig3), RooArgList(sig1frac, sig2frac), true);
	if (variation == "signal" && pdf == "gauss_cb") sig = new RooAddPdf(Form("sig_gaussCB%d_%s", _count, pdf.Data()), "", RooArgList(sig1, CB), sig1frac);

    if (!sig) {
        std::cerr
            << "ERROR: No signal PDF was created for "
            << tree
            << ", variation="
            << variation
            << ", pdf="
            << pdf
            << std::endl;

        return nullptr;
    	}

    nsigMC = new RooRealVar(
        Form("nsigMC%d_%s", _count, pdf.Data()),
        "",
        dsMC->sumEntries(),
        0.9 * dsMC->sumEntries(),
        1.1 * dsMC->sumEntries()
   		 );

    modelMC = new RooAddPdf(
        Form("modelMC%d_%s", _count, pdf.Data()),
        "",
        RooArgList(*sig),
        RooArgList(*nsigMC)
    	);

    scale->setConstant(true);

    fitResultMC = modelMC->fitTo(
        *dsMC,
        Save(),
        Extended(true),
        Range("signal"),
        Strategy(2),
        Offset(true)
    	);

	}


	//if (tree != "ntKstar") {RooFitResult* fitResultMC = modelMC->fitTo(*dsMC, Save(), Extended(), Range("signal"));}
	//else {RooFitResult* fitResultMC = modelMC->fitTo(*combData, Save(), Extended(kTRUE), Range("signal"));}

	// nsigMC is not part of the data model, so import it once here. The
	// complete signal graph is imported recursively with model below.
	w.import(*nsigMC);

	cMC->Clear();
	cMC->cd();

	const int signalColor = (tree == "ntmix_PSI2S") ? kOrange - 2 : kOrange - 3;

	double xMinPlot = mass->getMin();
	double xMaxPlot = mass->getMax();
	if (tree == "ntphi") { xMinPlot = 5.2; xMaxPlot = 5.55; }
	else if (tree == "ntKp") { xMinPlot = 5.05; xMaxPlot = 5.55; }
	else if (tree == "ntKstar") { xMinPlot = 5.05; xMaxPlot = 5.55; }

	TString xTtile_decayC = "";
	if (tree == "ntmix_X3872" || tree == "ntmix_PSI2S") xTtile_decayC = "m_{J/#psi #pi^{+} #pi^{-}} [GeV/c^{2}]";
	else if (tree == "ntphi") xTtile_decayC = "m_{J/#psi K^{+} K^{-}} [GeV/c^{2}]";
	else if (tree == "ntKstar") xTtile_decayC = "m_{J/#psi #pi^{+} K^{-}} [GeV/c^{2}]";
	else if (tree == "ntKp") xTtile_decayC = "m_{J/#psi K^{+}} [GeV/c^{2}]";

	if (tree == "ntKstar") {

		meanRT->setConstant(false);
		meanWT->setConstant(false);

		sigma1RT->setConstant(false);
		sigma2RT->setConstant(false);
		sigma3RT->setConstant(false);
		frac1RT->setConstant(false);
		frac2RT->setConstant(false);

		sigmaGWT->setConstant(false);
		sigmaLWT->setConstant(false);
		sigmaRWT->setConstant(false);
		fracWTG->setConstant(false);


		TPad* pMC_RT = new TPad(Form("pMC_RT_%d", _count), Form("pMC_RT_%d", _count), 0.0, 0.0, 0.5, 1.0);
		TPad* pMC_WT = new TPad(Form("pMC_WT_%d", _count), Form("pMC_WT_%d", _count), 0.5, 0.0, 1.0, 1.0);
		pMC_RT->SetBorderMode(1);
		pMC_RT->SetFrameBorderMode(0);
		pMC_RT->SetBorderSize(2);
		pMC_RT->SetBottomMargin(0.18);
		pMC_RT->SetLeftMargin(0.14);
		pMC_RT->SetRightMargin(0.04);
		pMC_RT->Draw();
		pMC_WT->SetBorderMode(1);
		pMC_WT->SetFrameBorderMode(0);
		pMC_WT->SetBorderSize(2);
		pMC_WT->SetBottomMargin(0.18);
		pMC_WT->SetLeftMargin(0.08);
		pMC_WT->SetRightMargin(0.14);
		pMC_WT->Draw();

		pMC_RT->cd();
		RooPlot* frameMC_RT = mass->frame(Range(5.10, 5.45));
		frameMC_RT->SetTitle("");
		frameMC_RT->GetYaxis()->SetTitle("Events / (10 MeV/c^{2})");
		frameMC_RT->GetYaxis()->SetTitleOffset(1.6);
		frameMC_RT->GetYaxis()->SetTitleSize(0.045);
		frameMC_RT->GetXaxis()->SetTitle(xTtile_decayC);
		frameMC_RT->GetXaxis()->SetTitleSize(0.035);
		frameMC_RT->GetXaxis()->SetTitleOffset(1.10);
		frameMC_RT->GetXaxis()->CenterTitle();
		frameMC_RT->GetYaxis()->SetTitleFont(42);
		frameMC_RT->GetXaxis()->SetLabelFont(42);
		frameMC_RT->GetYaxis()->SetLabelFont(42);
		frameMC_RT->GetXaxis()->SetLabelSize(0.030);
		frameMC_RT->GetYaxis()->SetLabelSize(0.030);
		frameMC_RT->SetStats(0);

		dsMC_RT->plotOn(frameMC_RT, Name(Form("dsMC_RT_%d_%s", _count, pdf.Data())), Binning(NBIN), MarkerSize(0.5), MarkerStyle(8), LineColor(1), LineWidth(1));
		shapeRT->plotOn(frameMC_RT, Name(Form("rtFit_%d_%s", _count, pdf.Data())), LineColor(kGreen + 2), LineWidth(2));
		shapeRT->paramOn(frameMC_RT, Layout(0.18, 0.45, 0.80), Format("NEU", AutoPrecision(2)));
		TPaveText* paramBoxRT = dynamic_cast<TPaveText*>(frameMC_RT->findObject(Form("%s_paramBox", shapeRT->GetName())));
		if (paramBoxRT) {
			paramBoxRT->SetTextSize(0.028);
			paramBoxRT->SetTextFont(42);
			paramBoxRT->SetFillStyle(0);
			paramBoxRT->SetBorderSize(0);
		}
		frameMC_RT->SetMinimum(0.0);
		frameMC_RT->SetMaximum(frameMC_RT->GetMaximum() * 1.5);
		frameMC_RT->Draw();
		TLatex* mesonNameMC = new TLatex(0.2, 0.88, FitParticleLabel(tree, true));
		setupLABELS(mesonNameMC, 0.060, true);
		cMC->RedrawAxis();

		TLatex* rtLabel = new TLatex(0.20, 0.84, "RT-only MC fit");
		setupLABELS(rtLabel, 0.045, true);
		TLegend* legRT = new TLegend(0.62, 0.75, 0.90, 0.88, NULL, "brNDC");
		legRT->SetBorderSize(0);
		legRT->SetTextSize(0.035);
		legRT->SetTextFont(42);
		legRT->SetFillStyle(0);
		legRT->AddEntry(frameMC_RT->findObject(Form("dsMC_RT_%d_%s", _count, pdf.Data())), "RT MC", "lp");
		legRT->AddEntry(frameMC_RT->findObject(Form("rtFit_%d_%s", _count, pdf.Data())), "RT fit", "l");
		legRT->Draw();

		pMC_WT->cd();
		RooPlot* frameMC_WT = mass->frame(Range(5.10, 5.45));
		frameMC_WT->SetTitle("");
		frameMC_WT->GetYaxis()->SetTitle("Events / (10 MeV/c^{2})");
		frameMC_WT->GetYaxis()->SetTitleOffset(1.6);
		frameMC_WT->GetYaxis()->SetTitleSize(0.045);
		frameMC_WT->GetXaxis()->SetTitle(xTtile_decayC);
		frameMC_WT->GetXaxis()->SetTitleSize(0.035);
		frameMC_WT->GetXaxis()->SetTitleOffset(1.10);
		frameMC_WT->GetXaxis()->CenterTitle();
		frameMC_WT->GetYaxis()->SetTitleFont(42);
		frameMC_WT->GetXaxis()->SetLabelFont(42);
		frameMC_WT->GetYaxis()->SetLabelFont(42);
		frameMC_WT->GetXaxis()->SetLabelSize(0.030);
		frameMC_WT->GetYaxis()->SetLabelSize(0.030);
		frameMC_WT->SetStats(0);
		dsMC_WT->plotOn(frameMC_WT, Name(Form("dsMC_WT_%d_%s", _count, pdf.Data())), Binning(NBIN), MarkerSize(0.5), MarkerStyle(8), LineColor(1), LineWidth(1));
		shapeWT->plotOn(frameMC_WT, Name(Form("wtFit_%d_%s", _count, pdf.Data())), LineColor(kMagenta + 1), LineWidth(2));
		shapeWT->paramOn(frameMC_WT, Layout(0.18, 0.45, 0.80), Format("NEU", AutoPrecision(2)));
		TPaveText* paramBoxWT = dynamic_cast<TPaveText*>(frameMC_WT->findObject(Form("%s_paramBox", shapeWT->GetName())));
		if (paramBoxWT) {
			paramBoxWT->SetTextSize(0.028);
			paramBoxWT->SetTextFont(42);
			paramBoxWT->SetFillStyle(0);
			paramBoxWT->SetBorderSize(0);
		}
		frameMC_WT->SetMinimum(0.0);
		frameMC_WT->SetMaximum(frameMC_RT->GetMaximum());
		frameMC_WT->Draw();
		setupLABELS(mesonNameMC, 0.060, true);
		cMC->RedrawAxis();
		TLatex* wtLabel = new TLatex(0.20, 0.84, "WT-only MC fit");
		setupLABELS(wtLabel, 0.045, true);
		TLegend* legWT = new TLegend(0.62, 0.75, 0.90, 0.88, NULL, "brNDC");
		legWT->SetBorderSize(0);
		legWT->SetTextSize(0.035);
		legWT->SetTextFont(42);
		legWT->SetFillStyle(0);
		legWT->AddEntry(frameMC_WT->findObject(Form("dsMC_WT_%d_%s", _count, pdf.Data())), "WT MC", "lp");
		legWT->AddEntry(frameMC_WT->findObject(Form("wtFit_%d_%s", _count, pdf.Data())), "WT fit", "l");
		legWT->Draw();
		DrawCmsHeader(cMC, system);
	    cMC->Update();

		meanRT->setConstant(true);
		meanWT->setConstant(true);

		sigma1RT->setConstant(true);
		sigma2RT->setConstant(true);
		sigma3RT->setConstant(true);
		frac1RT->setConstant(true);
		frac2RT->setConstant(true);

		sigmaGWT->setConstant(true);
		sigmaLWT->setConstant(true);
		sigmaRWT->setConstant(true);
		fracWTG->setConstant(true);

	} else {
		TPad* pMC1 = new TPad(Form("pMC1_%d", _count), Form("pMC1_%d", _count), 0., 0., 1., 1.);
		pMC1->SetBorderMode(1);
		pMC1->SetFrameBorderMode(0);
		pMC1->SetBorderSize(2);
		pMC1->SetBottomMargin(0.22);
		pMC1->SetLeftMargin(0.14);
		pMC1->SetRightMargin(0.04);
		pMC1->Draw();
		pMC1->cd();

		RooPlot* frameMC = mass->frame(Range(xMinPlot, xMaxPlot));
		frameMC->SetTitle("");
		frameMC->GetYaxis()->SetTitle(TString::Format("Events / (%g MeV/c^{2})", (mass->getMax() - mass->getMin()) / NBIN * 1000));
		frameMC->GetYaxis()->SetTitleOffset(2.15);
		frameMC->GetYaxis()->SetTitleSize(0.035);
		frameMC->GetXaxis()->SetTitle(xTtile_decayC);
		frameMC->GetXaxis()->SetTitleSize(0.030);
		frameMC->GetXaxis()->SetTitleOffset(1.10);
		frameMC->GetXaxis()->CenterTitle();
		frameMC->GetYaxis()->SetTitleFont(42);
		frameMC->GetXaxis()->SetLabelFont(42);
		frameMC->GetYaxis()->SetLabelFont(42);
		frameMC->GetXaxis()->SetLabelOffset(0.012);
		frameMC->GetXaxis()->SetLabelSize(0.031);
		frameMC->GetXaxis()->SetTickLength(0.035);
		frameMC->GetYaxis()->SetTitleSize(0.027);
		frameMC->GetYaxis()->SetLabelSize(0.027);
		frameMC->SetStats(0);

		dsMC->plotOn(frameMC, Name(Form("dsMC%d_%s", _count, pdf.Data())), Binning(NBIN), MarkerSize(0.5), MarkerStyle(8), LineColor(1), LineWidth(1));
		modelMC->plotOn(frameMC, Name(Form("sigMC%d_%s", _count, pdf.Data())), Range("signal"), NormRange("signal"), Normalization(nsigMC->getVal(), RooAbsReal::NumEvent), DrawOption("LF"), FillStyle(3002), FillColor(signalColor), LineStyle(7), LineColor(signalColor), LineWidth(1));
		modelMC->plotOn(frameMC, Name(Form("modelMCcurve%d_%s", _count, pdf.Data())), DrawOption("L"), LineWidth(0));
		modelMC->paramOn(frameMC, Layout(0.18, 0.48, 0.82), Format("NEU", AutoPrecision(2)));

		TPaveText* paramBoxMC =dynamic_cast<TPaveText*>(frameMC->findObject(Form("%s_paramBox", modelMC->GetName())));
		if (paramBoxMC) {
			paramBoxMC->SetTextSize(0.022);
			paramBoxMC->SetTextFont(42);
			paramBoxMC->SetFillStyle(0);
			paramBoxMC->SetBorderSize(0);
			paramBoxMC->SetY1NDC(0.72);
			paramBoxMC->SetY2NDC(0.82);
		}

		frameMC->getAttFill()->SetFillStyle(0);
		frameMC->getAttLine()->SetLineWidth(0);
		frameMC->SetMinimum(0.0);
		frameMC->SetMaximum(frameMC->GetMaximum() * 1.35);
		frameMC->Draw();
		TLatex* mesonNameMC = new TLatex(0.2, 0.88, FitParticleLabel(tree, true));
		setupLABELS(mesonNameMC, 0.060, true);
		cMC->RedrawAxis();

		TLegend* legMC = new TLegend(0.6, 0.78, 0.92, 0.90, NULL, "brNDC");
		legMC->SetBorderSize(0);
		legMC->SetTextSize(0.035);
		legMC->SetTextFont(42);
		legMC->SetFillStyle(0);
		legMC->AddEntry(frameMC->findObject(Form("dsMC%d_%s", _count, pdf.Data())), "Signal MC", "lp");
		legMC->AddEntry(frameMC->findObject(Form("sigMC%d_%s", _count, pdf.Data())), "Signal PDF", "f");
		legMC->Draw();
		DrawCmsHeader(cMC, system);
	    cMC->Update();
	}

	double n_signal_initial = ds->sumEntries(TString::Format("abs(Bmass-%g)<%g", init_mean, (tree == "ntmix_X3872" || tree == "ntmix_PSI2S") ? 0.005 : 0.05));
	const double nsigInit = std::max(0.0, n_signal_initial * 0.4);
	const double nsigMax = std::max(10.0, n_signal_initial * 2.0);
	RooRealVar nsig(Form("nsig%d_%s", _count, pdf.Data()), "", nsigInit, 0.0, nsigMax);

	RooRealVar nbkg(Form("nbkg%d_%s", _count, pdf.Data()), "", ds->sumEntries() * 0.7, ds->sumEntries() * 0.1, ds->sumEntries());
	RooRealVar a0(Form("a0%d_%s", _count, pdf.Data()), "", -0.35, -2, 2);
	RooRealVar a1(Form("a1%d_%s", _count, pdf.Data()), "", -0.05, -2, 2);
	RooRealVar a2(Form("a2%d_%s", _count, pdf.Data()), "", 0.01, -2, 2);
	RooRealVar a3(Form("a3%d_%s", _count, pdf.Data()), "", 0, -2, 2);
	RooRealVar a4(Form("a4%d_%s", _count, pdf.Data()), "", 0, -2, 2);
	RooChebychev bkg_2nd(Form("bkg%d_%s", _count, pdf.Data()), "", *mass, RooArgList(a0, a1));
	RooChebychev bkg_3rd(Form("bkg%d_%s", _count, pdf.Data()), "", *mass, RooArgSet(a0, a1, a2));
	RooChebychev bkg_4th(Form("bkg%d_%s", _count, pdf.Data()), "", *mass, RooArgSet(a0, a1, a2, a3));
	RooChebychev bkg_5th(Form("bkg%d_%s", _count, pdf.Data()), "", *mass, RooArgSet(a0, a1, a2, a3, a4));
	RooRealVar lambda(Form("lambda%d_%s", _count, pdf.Data()), "lambda", -0.5, -5., 0.);
	RooExponential bkg(Form("bkg%d_%s", _count, pdf.Data()), "", *mass, lambda);
	RooRealVar p_lin(Form("p_lin%d_%s", _count, pdf.Data()), "Linear slope", 0.0, -5.0, 5.0);
	RooPolynomial bkg_lin(Form("bkg_lin%d_%s", _count, pdf.Data()), "Linear Background", *mass, RooArgList(p_lin), 1);

	RooRealVar bkg_gauss1_mean(Form("bkg_gauss1_mean%d_%s", _count, pdf.Data()), "bkg_gauss1_mean", 5.25, 5.2, 5.4);
	RooRealVar bkg_gauss1_sigma(Form("bkg_gauss1_sigma%d_%s", _count, pdf.Data()), "bkg_gauss1_sigma", 0.043, 0.001, 0.1);
	RooGaussian bkg_gauss1(Form("bkg_gauss1%d_%s", _count, pdf.Data()), "bkg_gauss1", *mass, bkg_gauss1_mean, bkg_gauss1_sigma);
    RooRealVar nbkg_gauss1(Form("nbkg_gauss1%d_%s", _count, pdf.Data()), "", ds->sumEntries() * 500, 0.0, ds->sumEntries());

	RooRealVar bkg_gauss2_mean(Form("bkg_gauss2_mean%d_%s", _count, pdf.Data()), "bkg_gauss2_mean", 5.44, 5.3, 5.5);
	RooRealVar bkg_gauss2_sigma(Form("bkg_gauss2_sigma%d_%s", _count, pdf.Data()), "bkg_gauss2_sigma", 0.098, 0.001, 0.2);
	RooGaussian bkg_gauss2(Form("bkg_gauss2%d_%s", _count, pdf.Data()), "bkg_gauss2", *mass, bkg_gauss2_mean, bkg_gauss2_sigma);
	RooRealVar nbkg_gauss2(Form("nbkg_gauss2%d_%s", _count, pdf.Data()), "", ds->sumEntries() * 500, 0.0, ds->sumEntries());

	RooRealVar nbkg_part_r(Form("nbkg_part_r%d_%s", _count, pdf.Data()), "", 3000, 0, 10000);
	RooRealVar* m_nonprompt_scale = new RooRealVar(Form("m_nonprompt_scale%d_%s", _count, ""), "m_nonprompt_scale", 0.02, 0.005, 0.05);
	RooRealVar* m_nonprompt_shift = new RooRealVar(Form("m_nonprompt_shift%d_%s", _count, ""), "m_nonprompt_shift", 5.13, 5.12, 5.14);


	// Fixing the parameters of the background PDFs based on the results of the fit to the inclusive data sample
    //RooRealVar* m_nonprompt_scale = new RooRealVar(Form("m_nonprompt_scale%d_%s", _count, ""), "m_nonprompt_scale", 0.0464);
	//m_nonprompt_scale->setConstant(true);
	//RooRealVar* m_nonprompt_shift = new RooRealVar(Form("m_nonprompt_shift%d_%s", _count, ""), "m_nonprompt_shift", 5.1362);
    //m_nonprompt_shift->setConstant(true);

	RooGenericPdf* erfc = new RooGenericPdf(Form("erfc%d", _count), "0.5*TMath::Erfc((@0-@2)/@1)", RooArgList(*mass, *m_nonprompt_scale, *m_nonprompt_shift));

	RooAddPdf* model = nullptr;
	if (tree == "ntmix_X3872" || tree == "ntmix_PSI2S") {
		if ((variation == "" && pdf == "") || variation == "signal") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(*sig, bkg_2nd), RooArgList(nsig, nbkg));
		if (variation == "background" && pdf == "3rd") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(*sig, bkg_3rd), RooArgList(nsig, nbkg));
	}
	if (tree == "ntphi") {
		if ((variation == "" && pdf == "") || variation == "signal") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(*sig, bkg_2nd), RooArgList(nsig,nbkg));
		//if (variation == "background" && pdf == "2nd") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(*sig, bkg_2nd), RooArgList(nsig, nbkg));
		if (variation == "background" && pdf == "exp") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(*sig, bkg), RooArgList(nsig, nbkg));
		if (variation == "background" && pdf == "linear") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(*sig, bkg_lin), RooArgList(nsig, nbkg));
		if (variation == "background" && pdf == "4th") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(*sig, bkg_4th), RooArgList(nsig, nbkg));
		if (variation == "background" && pdf == "5th") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(*sig, bkg_5th), RooArgList(nsig, nbkg));
	}
	if (tree == "ntKstar") {
		if ((variation == "" && pdf == "") || variation == "signal") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(*sig, bkg_3rd), RooArgList(nsig, nbkg));
		if (variation == "background" && pdf == "2nd") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(*sig, bkg_2nd), RooArgList(nsig, nbkg));
		//if (variation == "background" && pdf == "3rd") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(*sig, bkg_3rd), RooArgList(nsig, nbkg));
		if (variation == "background" && pdf == "exp") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(*sig, bkg), RooArgList(nsig, nbkg));
		if (variation == "background" && pdf == "linear") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(*sig, bkg_lin), RooArgList(nsig, nbkg));
		if (variation == "signal" && pdf == "1gauss") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(*sig, bkg_4th), RooArgList(nsig, nbkg));
		if (variation == "signal" && pdf == "3gauss") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(*sig, bkg_4th), RooArgList(nsig, nbkg));
		if (variation == "background" && pdf == "4th") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(*sig, bkg_4th), RooArgList(nsig, nbkg));
		if (variation == "background" && pdf == "5th") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(*sig, bkg_5th), RooArgList(nsig, nbkg));

	}
	if (tree == "ntKp") {
		if ((variation == "" && pdf == "")) model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(*sig, bkg, *erfc), RooArgList(nsig, nbkg, nbkg_part_r));
		if (pdf == "mass_range") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(*sig, bkg), RooArgList(nsig, nbkg));
		if (variation == "background" && pdf == "linear") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(bkg_lin, *sig, *erfc), RooArgList(nbkg, nsig, nbkg_part_r));
		if (variation == "background" && pdf == "2nd") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(bkg_2nd, *sig, *erfc), RooArgList(nbkg, nsig, nbkg_part_r));
		if (variation == "background" && pdf == "3rd") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(bkg_3rd, *sig, *erfc), RooArgList(nbkg, nsig, nbkg_part_r));
		if (variation == "signal" && pdf == "1gauss") model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(bkg, sig1, *erfc), RooArgList(nbkg, nsig, nbkg_part_r));
		if (variation == "signal" && (pdf == "3gauss" || pdf == "fixed" || pdf == "gauss_cb")) model = new RooAddPdf(Form("model%d_%s", _count, pdf.Data()), "", RooArgList(*sig, bkg, *erfc), RooArgList(nsig, nbkg, nbkg_part_r));
	}
	

		scale->setConstant(false);
		sigma1.setConstant();
		if (pdf != "1gauss") {
			sigma2.setConstant();
			sig1frac.setConstant();
		}
		if (variation == "signal" && pdf == "3gauss") {
			sigma3.setConstant();
			sig2frac.setConstant();
		}
		if (variation == "signal" && pdf == "gauss_cb") {
			sigma4cb.setConstant();
			n.setConstant();
			alpha.setConstant();
		}
		if (variation == "signal" && pdf == "fixed") mean.setConstant();

	

	if (!model) {
		std::cerr << "ERROR: No data-fit model for tree=" << tree
			<< ", variation=" << variation << ", pdf=" << pdf << std::endl;
		return nullptr;
	}

	TString fitRange = (pdf == "mass_range") ? "m_rangeB" : "all";
	RooFitResult* fitResult = model->fitTo(*ds, Save(), Extended(kTRUE), Range(fitRange));
	if (!fitResult) {
		std::cerr << "ERROR: Data fit returned no result for tree=" << tree
			<< ", variation=" << variation << ", pdf=" << pdf << std::endl;
		return nullptr;
	}
	fitResult->Print("v");
	w.import(*model);

	c->cd();
	RooPlot* frame = mass->frame(Range(xMinPlot, xMaxPlot));
	frame->SetStats(0);
	frame->SetTitle("");
	frame->GetYaxis()->SetTitle(TString::Format("Events / (%g MeV/c^{2})", (mass->getMax() - mass->getMin()) / NBIN * 1000));
	frame->GetYaxis()->SetTitleOffset(2.);
	frame->GetXaxis()->SetTitleOffset(1.2);
	frame->GetYaxis()->SetTitleSize(0.035);
	frame->GetXaxis()->SetTitleSize(0.);
	frame->GetXaxis()->SetTitleFont(0);
	frame->GetYaxis()->SetTitleFont(42);
	frame->GetXaxis()->SetLabelFont(42);
	frame->GetYaxis()->SetLabelFont(42);
	frame->GetXaxis()->SetLabelSize(0);
	frame->GetYaxis()->SetLabelSize(0.035);

	TPad* p1 = new TPad("p1", "p1", 0., 0.22, 1., 1);
	p1->SetBorderMode(1);
	p1->SetFrameBorderMode(0);
	p1->SetBorderSize(2);
	p1->SetBottomMargin(0.01);
	p1->SetLeftMargin(0.14);
	p1->SetRightMargin(0.04);
	p1->Draw();
	TPad* p2 = new TPad("p2", "p2", 0., 0., 1., 0.22);
	p2->SetTopMargin(0.0);
	p2->SetBottomMargin(0.34);
	p2->SetLeftMargin(0.14);
	p2->SetRightMargin(0.04);
	p2->SetBorderMode(0);
	p2->SetBorderSize(2);
	p2->SetFrameBorderMode(0);
	p2->SetTicks(1, 1);
	p2->Draw();
	c->cd();

	p1->cd();
	ds->plotOn(frame, Name(Form("ds_cut%d", _count)), Binning(NBIN), MarkerSize(0.5), MarkerStyle(8), MarkerColor(1), LineColor(1), LineWidth(1));
	model->plotOn(frame, Name(Form("model%d_%s", _count, pdf.Data())), Range(fitRange), NormRange(fitRange), Precision(1e-6), DrawOption("L"), LineColor(2), LineWidth(1));
	model->plotOn(frame, Name(Form("sig%d_%s", _count, pdf.Data())), Components(*sig), DrawOption("LF"), FillStyle(3002), FillColor(signalColor), LineStyle(7), LineColor(signalColor), LineWidth(1));
	if (tree == "ntKp") model->plotOn(frame, RooFit::Name(Form("erfc%d_%s", _count, "")), Components(*erfc), Range(fitRange), NormRange(fitRange), LineColor(kGreen + 3), LineStyle(9), LineWidth(2), DrawOption("L"));
	if (tree == "ntKstar") { 
		model->plotOn(frame, Name(Form("rtComp%d_%s", _count, pdf.Data())), Components(*shapeRT), Range(fitRange), NormRange(fitRange), LineColor(kGreen + 2), LineStyle(kDashed), LineWidth(2));
		model->plotOn(frame, Name(Form("wtComp%d_%s", _count, pdf.Data())), Components(*shapeWT), Range(fitRange), NormRange(fitRange), LineColor(kMagenta + 1), LineStyle(kDashed), LineWidth(2));
	}
	model->plotOn(frame, Name(Form("bkg%d_%s", _count, pdf.Data())), Components(bkg), Range(fitRange), NormRange(fitRange), Precision(1e-6), DrawOption("L"), LineStyle(7), LineColor(4), LineWidth(1));
	double chi2Ndf = frame->chiSquare(Form("model%d_%s", _count, pdf.Data()), Form("ds_cut%d", _count), fitResult->floatParsFinal().getSize());
	if (!std::isfinite(chi2Ndf) || chi2Ndf < 0) chi2Ndf = -1.0;
	RooRealVar chi2Var(Form("chi2_data_norm%d_%s", _count, pdf.Data()), "", chi2Ndf);
	w.import(chi2Var);
	model->paramOn(frame, Layout(0.18, 0.48, 0.84), Format("NEU", AutoPrecision(2)));

    TPaveText* paramBox =
    dynamic_cast<TPaveText*>(frame->findObject(Form("%s_paramBox", model->GetName())));

	if (paramBox) {
    	paramBox->SetTextSize(0.022);
    	paramBox->SetTextFont(42);
    	paramBox->SetFillStyle(0);
    	paramBox->SetBorderSize(0);
	}

	if (tree == "ntKstar" && paramBox) {

    const double fWT = fWTMC->getVal();

    const double nRTData = (1.0 - fWT) * nsig.getVal();

    const double nWTData = fWT * nsig.getVal();

    const double nRTDataErr = (1.0 - fWT) * nsig.getError();

    const double nWTDataErr = fWT * nsig.getError();

    paramBox->AddText(Form("N_{RT} = %.0f #pm %.0f",nRTData,nRTDataErr));

    paramBox->AddText(Form("N_{WT} = %.0f #pm %.0f", nWTData, nWTDataErr));

    paramBox->AddText(Form("f_{WT} = %.3f", fWT));
	}

	frame->getAttFill()->SetFillStyle(0);
	frame->getAttLine()->SetLineWidth(0);
	frame->SetMinimum(0.0);
	frame->SetMaximum(frame->GetMaximum() * 1.60);
	frame->Draw();

	TLegend* leg = new TLegend(0.67, 0.60, 0.91, 0.90, NULL, "brNDC");
	if (tree != "ntKp" && tree != "ntmix_X3872") leg = new TLegend(0.68, 0.66, 0.92, 0.90, NULL, "brNDC");
	leg->SetBorderSize(0);
	leg->SetFillStyle(0);
	leg->SetTextSize(0.035);
	leg->SetTextFont(42);
	leg->AddEntry(frame->findObject(Form("ds_cut%d", _count)), "Data", "LEP");
	leg->AddEntry(frame->findObject(Form("model%d_%s", _count, pdf.Data())), "Fit Model", "l");
	leg->AddEntry(frame->findObject(Form("bkg%d_%s", _count, pdf.Data())), " Comb. Bkg.", "l");
	if (tree == "ntKp") {
		leg->AddEntry(frame->findObject(Form("sig%d_%s", _count, pdf.Data())), " B^{+} #rightarrow J/#psi K^{+}", "f");
		leg->AddEntry(frame->findObject(Form("erfc%d_%s", _count, pdf.Data())), " B #rightarrow J/#psi X", "l");
	} else if (tree == "ntmix_X3872") {
		leg->AddEntry(frame->findObject(Form("sig%d_%s", _count, pdf.Data())), " X(3872) #rightarrow J/#psi #pi^{+} #pi^{-}", "f");
	} else if (tree == "ntmix_PSI2S") {
		leg->AddEntry(frame->findObject(Form("sig%d_%s", _count, pdf.Data())), " #psi(2S) #rightarrow J/#psi #pi^{+} #pi^{-}", "f");
	} else if (tree == "ntphi") {
		leg->AddEntry(frame->findObject(Form("sig%d_%s", _count, pdf.Data())), " B_{s}^{0} #rightarrow J/#psi K^{+}K^{-}", "f");
	} else if (tree == "ntKstar") {
		leg->AddEntry(frame->findObject(Form("sig%d_%s", _count, pdf.Data())), " B^{0} #rightarrow J/#psi K^{*}", "f");
		leg->AddEntry(frame->findObject(Form("rtComp%d_%s", _count, pdf.Data())), "RT component", "l");
		leg->AddEntry(frame->findObject(Form("wtComp%d_%s", _count, pdf.Data())), "WT component", "l");
	}
	leg->Draw();
	DrawCmsHeader(cMC, system);
	c->Update();


	p2->cd();
	RooHist* pull_hist = frame->pullHist(Form("ds_cut%d", _count), Form("model%d_%s", _count, pdf.Data()));
	pull_hist->SetMarkerSize(0.5);
	RooPlot* pull_plot = mass->frame(Range(xMinPlot, xMaxPlot));
	pull_plot->addPlotable(static_cast<RooPlotable*>(pull_hist), "XP");
	pull_plot->SetTitle("");
	pull_plot->SetXTitle(xTtile_decayC.Data());
	pull_plot->SetYTitle("Pull");
	pull_plot->GetYaxis()->SetTitleFont(42);
	pull_plot->GetYaxis()->SetTitleSize(0.13);
	pull_plot->GetYaxis()->CenterTitle(kTRUE);
	pull_plot->GetYaxis()->SetLabelOffset(0.01);
	pull_plot->GetYaxis()->SetLabelFont(42);
	pull_plot->GetYaxis()->SetLabelSize(0.12);
	pull_plot->GetYaxis()->SetNdivisions(305);
	pull_plot->GetYaxis()->SetTitleOffset(0.55);
	pull_plot->GetYaxis()->SetRangeUser(-3.5, 3.5);
	pull_plot->GetXaxis()->SetTitleSize(0.13);
	pull_plot->GetXaxis()->SetTitleOffset(1.0);
	pull_plot->GetXaxis()->CenterTitle();
	pull_plot->GetXaxis()->SetLabelFont(42);
	pull_plot->GetXaxis()->SetLabelOffset(0.01);
	pull_plot->GetXaxis()->SetLabelSize(0.14);
	pull_plot->GetXaxis()->SetTickLength(0.16);
	pull_plot->Draw();
	TLine* line_ref = new TLine(xMinPlot, 0., xMaxPlot, 0.);
	line_ref->SetLineStyle(1);
	line_ref->SetLineColor(2);
	line_ref->SetLineWidth(1);
	line_ref->Draw("same");

	cout << "\n-------------------------------------------------------------------------------------- \n" << endl;
	cout << "Signal Yield Y_s = " << nsig.getVal() << "     yield Error = " << nsig.getError();
	cout << "\n-------------------------------------------------------------------------------------- \n" << endl;

	p1->cd();
	return fitResult;
}
// END OF MAIN FITTING FUNCTION
