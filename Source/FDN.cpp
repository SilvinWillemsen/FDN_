/*
  ==============================================================================

    FDN.cpp
    Created: 13 Nov 2019 9:22:01am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "FDN.h"

//==============================================================================
FDN::FDN()
{
    FDNorder = Global::initFDNorder;
    RT.resize (Global::numOctaveBands, Global::RT);
    constructor (Global::initMatType);
}

void FDN::constructor (MatrixType matType)
{
    b.resize (FDNorder, 1.0);
    c.resize (FDNorder, 1.0);
    
    // initialise scattering matrix
    setScatteringMatrix (matType);

}

FDN::~FDN()
{
    
}

void FDN::paint (Graphics& g)
{

}

void FDN::resized()
{
}

double FDN::calculate (double input)
{
    output = 0;
    std::vector<double> intermediateOutput (FDNorder, 0);
    
    for (int i = 0; i < FDNorder; ++i)
    {
        intermediateOutput[i] = c[i] * eqCombs[i]->filter (b[i] * input);
        for (int j = 0; j < FDNorder; ++j)
        {
            eqCombs[j]->addScatOutput (A[j][i] * intermediateOutput[i]);
        }

        output += intermediateOutput[i];
    // calculate using output of scattering matrix
    // eqComb->filter
    
    
    // output of eqComb goes out and back into scattering matrix
        
    }

    for (int i = 0; i < FDNorder; ++i)
    {
        eqCombs[i]->increment();
        eqCombs[i]->zeroWritePointer();
    }
    
//    if (output != 0)
//    {
//        for (int i = 0; i < intermediateOutput.size(); ++i)
//            std::cout << intermediateOutput[i] << " ";
//        std::cout << std::endl;
//    }
    return output;
}

void FDN::initialise (double sampleRate)
{
    fs = sampleRate;
    
    gainDB.clear();
    gainDB.resize (FDNorder, std::vector<double> (Global::numOctaveBands, 0));
    numsOpt.resize (3, std::vector<double> (Global::numOctaveBands + 1, 0));
    densOpt.resize (3, std::vector<double> (Global::numOctaveBands + 1, 0));
    
    Goptdb.resize (Global::numOctaveBands, 0);
    Gopt.resize (Global::numOctaveBands, 0);
    G2optdb.resize (Global::numOctaveBands, 0);
    G2opt.resize (Global::numOctaveBands, 0);
    G2woptdb.resize (Global::numOctaveBands, 0);
    G2wopt.resize (Global::numOctaveBands, 0);
    
    leak.resize (Global::numOctaveBands, std::vector<double> (Global::numDesignFreqs, 0));
    leak2.resize (Global::numOctaveBands, std::vector<double> (Global::numDesignFreqs, 0));
    
    dLen.resize (FDNorder, 0);
	getDelayLines(); // DO NOT recalculate delay lines each time

    recalculateCoeffs (true);
    
    std::vector<double> vals;
    
    for (int i = 0; i < FDNorder; ++i)
    {
        for (int j = 0; j < Global::numOctaveBands + 1; ++j)
        {
            std::cout << "Comb filter " << i+1 << ", octaveband " << j+1 << " Numerators: ";
            vals = eqCombs[i]->getFilter(j)->getNums();
            for (int k = 0; k < 3; ++k)
                std::cout << vals[k] << " ";
            std::cout << std::endl << "Denominators: ";
            vals = eqCombs[i]->getFilter(j)->getDens();
            for (int k = 0; k < 3; ++k)
                std::cout << vals[k] << " ";
            
            std::cout << std::endl;
        }
    }
    std::cout << "YAY" << std::endl;
    initialised = true;
}

void FDN::recalculateCoeffs (bool init)
{
//    zeroCoefficients(); // makes sure that the states of the filters are and delay lines reset 
    getAttenuation();
    
    if (init)
    {
        for (int i = 0; i < FDNorder; ++i)
        {
            eqCombs.add (new EQComb (dLen[i]));
        }
    }
    
    for (int i = 0; i < FDNorder; ++i)
    {
        aceq (i);
        for (int j = 0; j < Global::numOctaveBands + 1; ++j)
            eqCombs[i]->setFilter (j, std::vector<double> {
                numsOpt[0][j], numsOpt[1][j], numsOpt[2][j],
                densOpt[0][j], densOpt[1][j], densOpt[2][j]
            });
    }
}

std::vector<double> FDN::pareq (double G, double GB, double w0, double bw)
{
    double beta;
    if (G == 1)
        beta = tan(bw / 2.0); // To avoid division by zero, when G = 1
    else
        beta = sqrt(abs(GB * GB - 1) / abs(G*G - GB*GB)) * tan(bw / 2.0);
    
    //num[0]
    double num1 = (1.0 + G * beta) / (1.0 + beta);
    //num[1]
    double num2 = (-2.0 * cos(w0)) / (1.0 + beta);
    //num[2]
    double num3 = (1.0 - G * beta)/(1.0 + beta);
    
    //den[0]
    double den1 = 1.0;
    //den[1]
    double den2 = (-2.0 * cos(w0)) / (1.0 + beta);
    //den[2]
    double den3 = (1.0 - beta) / (1.0 + beta);
    
    return std::vector<double> {num1, num2, num3, den1, den2, den3};
}

void FDN::interactionMatrix (double* G, double gw, double *wg, double *wc, double *bw)
{
    std::vector<std::complex<double>> dummyVect (Global::numOctaveBands, 0);
    //leak = zeros(M, N);// Initialize interaction matrix
    std::vector<double> Gdb (Global::numOctaveBands, 0);
    std::vector<double> Gdbw (Global::numOctaveBands, 0);
    std::vector<double> Gw (Global::numOctaveBands, 0);
    double sumG = 0;
    std::vector<double> Gain (Global::numOctaveBands, 0);
    
    std::complex<double> i (0, -1);
    
    //    std::vector<std::vector<double>> coeffs (6, dummyVect);
    
    for (int m = 0; m < Global::numOctaveBands; m++)
    {
        Gdb[m] = 20 * log10 (G[m]); // Convert linear gain factor to dB
        Gdbw[m] = gw * Gdb[m]; // dB gain at bandwidth
        Gw[m] = pow(10, (Gdbw[m] / 20)); // Convert to linear gain factors
        sumG = sumG + Gdb[m];
    }
    
    //Estimate leak factors of peak / notch filters
    std::vector<double> coeffs (6, 0); // for one frequency band
    std::vector<std::complex<double>> complexCoeffs (6, 0); // for one frequency band
    
    std::complex<double> H;
    if (abs(sumG) != 0)
    {
        for (int m = 0; m < Global::numOctaveBands; m++)
        {// Filters
            coeffs = pareq (G[m], Gw[m], wg[m], bw[m]);//Parametric EQ filter
            for (int i = 0; i < coeffs.size(); ++i)
                complexCoeffs[i].real (coeffs[i]);
            
            for (int k = 0; k < Global::numDesignFreqs; k++) // wc is vector of central frqs, so the sized is 10 - most liekly, in this case at least
            {
                H = (complexCoeffs[0] + complexCoeffs[1] * exp(-i * wc[k]) + complexCoeffs[2] * exp(-2.0 * i * wc[k])) / (complexCoeffs[3] + complexCoeffs[4] * exp(-i * wc[k]) + complexCoeffs[5] * exp(-2.0 * i * wc[k]));
                leak[m][k] = abs (20.0 * log10 (H) / Gdb[m]);
            }
        }
    }
}

void FDN::aceq (int idx)
{
    int halfOctaveBands = floor (Global::numOctaveBands * 0.5);
	double G0 = (gainDB[idx][halfOctaveBands] + gainDB[idx][halfOctaveBands + 1]) * 0.5;
    
    for (int i = 0; i < Global::numOctaveBands; i++)
        gainDB[idx][i] = -G0 + gainDB[idx][i];
    
    // wg = [.0044 .009 .0178 .0356 .0712 .01425 0.2850 0.5699 1.1398 2.28];
    wg.resize (Global::numOctaveBands, 0);
    wc.resize (Global::numDesignFreqs, 0);
    for (int i = 0; i < Global::numOctaveBands; i++)
        wg[i] = 2.0 * double_Pi * Global::fc1[i] / fs; // Command gain frequencies in radians
    
    for (int j = 0; j < Global::numDesignFreqs; j++)
        wc[j] = 2.0 * double_Pi * fc2[j] / fs; // Center frequencies in radians for iterative design with extra points
    
    bw.resize (Global::numOctaveBands, 0); // EQ filter bandwidths
    
    for (int m = 0; m < Global::numOctaveBands; m++)
        bw[m] = 1.5 * wg[m];
    
    if (Global::numOctaveBands == 10)
    {
        bw[Global::numOctaveBands - 3] = 0.93 * bw[Global::numOctaveBands - 3];
        bw[Global::numOctaveBands - 2] = 0.78 * bw[Global::numOctaveBands - 2];
        bw[Global::numOctaveBands - 1] = 0.76 * wg[Global::numOctaveBands - 1]; // Additional adjustmenst due to asymmetry
    }
    
    inG.resize(Global::numOctaveBands, cc);
    interactionMatrix (&inG[0], gw, &wg[0], &wc[0], &bw[0]); //Estimate leakage b / w bands
    
    Gdb2.resize(Global::numDesignFreqs, 0);
    
    for (int k = 0; k < Global::numDesignFreqs; ++k)
        if (k % 2 == 0)
            Gdb2[k] = gainDB[idx][k / 2];
        else
            Gdb2[k] = (Gdb2[k-1] + Gdb2[k+1]) / 2.0; // Interpolate target gains linearly b / w command gains
    
    //// Linear least squares ////
    
    // transpose matrix
    for (int i = 0; i < Global::numOctaveBands; i++)
        for (int j = 0; j < Global::numDesignFreqs; j++)
            leakMat(j, i) = leak[i][j];
    
    // Linear least squares
    Gdb2Vect = Eigen::VectorXd::Map (&Gdb2[0], Gdb2.size());
    Gdb2Vect = Gdb2Vect.transpose();
    solution = leakMat.bdcSvd (Eigen::ComputeThinU | Eigen::ComputeThinV).solve(Gdb2Vect);
    
    for (int i = 0; i < Global::numOctaveBands; ++i)
        Goptdb[i] = solution(i);
    
    for (int k = 0; k < Global::numOctaveBands; k++)
        Gopt[k] = pow(10, Goptdb[k] / 20.0); // Convert to linear gain factors
    
    // (Second interaction matrix) Iterate once
    interactionMatrix (&Gopt[0], gw, &wg[0], &wc[0], &bw[0]); // Use previous gains
    
    // transpose matrix
    for (int i = 0; i < Global::numOctaveBands; i++)
        for (int j = 0; j < Global::numDesignFreqs; j++)
            leakMat2(j, i) = leak[i][j];
    
    // Linear least squares
    G2optdbVect = Eigen::VectorXd::Map (&Gdb2[0], Gdb2.size());
    G2optdbVect = G2optdbVect.transpose();
    solution2 = leakMat2.bdcSvd (Eigen::ComputeThinU | Eigen::ComputeThinV).solve(G2optdbVect);
    
    for (int i = 0; i < Global::numOctaveBands; ++i)
        G2optdb[i] = solution2 (i);
    
    for (int k = 0; k < Global::numOctaveBands; k++)
    {
        G2opt[k] = pow (10, G2optdb[k] / 20.0); // Convert to linear gain factors
        G2woptdb[k] = gw * G2optdb[k];        // Gain at bandwidth wg
        G2wopt[k] = pow(10, G2woptdb[k] / 20.0); //Convert to linear gain factor
    }
    
    // Design filters with optimized gains
    coeffsFromPareq.resize (6, 0);
    for (int k = 0; k < Global::numOctaveBands; k++)
    {
        coeffsFromPareq = pareq (G2opt[k], G2wopt[k], wg[k], bw[k]); // Design filters
        
        numsOpt[0][k] = coeffsFromPareq[0] * pow(10 ,(G0 / 200.0));
        numsOpt[1][k] = coeffsFromPareq[1] * pow(10, (G0 / 200.0));
        numsOpt[2][k] = coeffsFromPareq[2] * pow(10, (G0 / 200.0));
        densOpt[0][k] = coeffsFromPareq[3];
        densOpt[1][k] = coeffsFromPareq[4];
        densOpt[2][k] = coeffsFromPareq[5];
    }
    
    // High Shelving filter
	double GHdb = gainDB[idx][Global::numOctaveBands - 1];// min(gDB); There should be no "-G0" here
    double fH = 20200.f; // 20200;
    double GH = pow(10, GHdb / 20.0); // linear gain
    double wH = 2.0 * double_Pi * fH / fs; // frequency in[rad]
    
    //  coefficients
    double aH0 = sqrt(GH) * tan(wH * 0.5) + 1;
    double aH1 = sqrt(GH) * tan(wH * 0.5) - 1;
    double bH0 = sqrt(GH) * tan(wH * 0.5) + GH;
    double bH1 = sqrt(GH) * tan(wH * 0.5) - GH;
    
    
    numsOpt[0][Global::numOctaveBands] = bH0 / aH0;
    numsOpt[1][Global::numOctaveBands] = bH1 / aH0;
    densOpt[0][Global::numOctaveBands] = aH0 / aH0;
    densOpt[1][Global::numOctaveBands] = aH1 / aH0;
    
}

void FDN::getDelayLines()
{
    dLen.clear();
    dLen.resize (FDNorder, 0);
    if (Global::usePredefinedDLens)
    {
        for (int i = 0; i < FDNorder; ++i)
            dLen[i] = Global::dLens[i];
    }
    else
    {
        Random rand;
        // make a random-length delay line
        for (int i = 0; i < FDNorder; ++i)
        {
            dLen[i] = round(Global::minDelayLength + (Global::maxDelayLength - Global::minDelayLength) * rand.nextFloat());
            std::cout << i << " " << dLen[i] << std::endl;
        }
    }
}

int FDN::getClosestToAvgDLenIdx()
{
    // find closest delayline length
    double avgDLen = getAvgDLen();
    int closestDLen = 0;
    int diffDLen = 10000;
    for (int i = 0; i < FDNorder; ++i)
    {
        if (abs(avgDLen - dLen[i]) < diffDLen)
        {
            closestDLen = i;
            diffDLen = abs(avgDLen - dLen[i]);
        }
    }
    return closestDLen;
}

void FDN::getAttenuation()
{
       
    // get attenuation depending on length of the delay line
    for (int k = 0; k < FDNorder; k++)
    {
        for (int m = 0; m < Global::numOctaveBands; m++)
        {
            gainDB[k][m] = (-60.0 / (RT[m] * fs)) * dLen[k];
        }
    }
}

void FDN::debugPrint (std::vector<std::vector<double>>& vect)
{
    for (int i = 0; i < vect.size(); ++i)
    {
        for (int j = 0; j < vect[i].size(); ++j)
        {
            std::cout << vect[i][j];
            std::cout << "  ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

double FDN::getAvgDLen()
{
    double tmp = 0;
    for (int i = 0; i < dLen.size(); ++i)
    {
        tmp += dLen[i];
    }
    return tmp / static_cast<double> (dLen.size());
}

double FDN::getMaxDLen()
{
    double tmp = 0;
    for (int i = 0; i < dLen.size(); ++i)
    {
        if (dLen[i] > tmp)
        tmp = dLen[i];
    }
    return tmp;
}

int FDN::getMaxDLenIdx()
{
    int idx = 0;
    int tmp = 0;
    for (int i = 0; i < dLen.size(); ++i)
    {
        if (dLen[i] > tmp)
        {
            tmp = dLen[i];
            idx = i;
        }
    }
    return idx;
}


double FDN::getMinDLen()
{
    int tmp = Global::maxDelayLength;
    for (int i = 0; i < dLen.size(); ++i)
    {
        if (dLen[i] < tmp)
            tmp = dLen[i];
    }
    return tmp;
}

int FDN::getMinDLenIdx()
{
    int tmp = Global::maxDelayLength;
    int idx;
    for (int i = 0; i < dLen.size(); ++i)
    {
        if (dLen[i] < tmp)
        {
            tmp = dLen[i];
            idx = i;
        }
    }
    return idx;
}


void FDN::setScatteringMatrix (MatrixType matType)
{
    A.clear();
    A.resize (FDNorder, std::vector<double> (FDNorder, 0));

    double scaleFactor = 1.0 / static_cast<double> (sqrt(FDNorder));
    switch (matType)
    {
        case householder:
        {
//            if (log(FDNorder) / log(4) == floor(log(FDNorder) / log(4))) // if the order is of the form 4^n where n is an integer
            if (FDNorder == 16)
			{
				std::vector<std::vector<double>> tmpMatrix (4, std::vector<double>(4, -1));
				for (int i = 0; i < 4; ++i)
					tmpMatrix[i][i] = 1;

				for (int i = 0; i < FDNorder; ++i)
				{
					for (int j = 0; j < FDNorder; ++j)
					{
						if (i / 4 == j / 4)

							A[i][j] = scaleFactor * tmpMatrix[i % 4][j % 4];
						else
							A[i][j] = -scaleFactor * tmpMatrix[i % 4][j % 4];
					}
				}
			}
			else
			{
                double scF = 2.0 / static_cast<double> (FDNorder);
				std::vector<std::vector<double>> tmpMatrix (FDNorder, std::vector<double>(FDNorder, scF));
				for (int i = 0; i < FDNorder; ++i)
				{
					for (int j = 0; j < FDNorder; ++j)
					{
						if (i == j)

							A[i][j] = 1 - tmpMatrix[i][j];
						else
							A[i][j] = -tmpMatrix[i][j];
					}
				}
			}
            std::cout << "householder" << std::endl;
			break;
        }
        case hadamard:
        {
            A[0][0] = 1.0f * scaleFactor;
            for (int n = 1; n < FDNorder; n += n)
            {
                for (int i = 0; i < n; ++i)
                {
                    for (int j = 0; j < n; ++j)
                    {
                        A[i + n][j] = A[i][j];
                        A[i][j + n] = A[i][j];
                        A[i + n][j + n] = - A[i][j];
                    }
                }
            }
            std::cout << "hadamard" << std::endl;
            break;
        }
		case identity:
		{
			for (int i = 0; i < FDNorder; ++i)
			{
				for (int j = 0; j < FDNorder; ++j)
				{
					if (i == j)
						A[i][j] = 1;
				}
			}
			break;
		}
        case randomMat:
        {
            Eigen::MatrixXd randMat (FDNorder, FDNorder);       // initialise empty matrix
            randMat.setRandom();                                // fill matrix with random values
            Eigen::HouseholderQR<Eigen::MatrixXd> qr (randMat); // perform QR decomposition
            Eigen::MatrixXd Q = qr.householderQ();              // get Q (orthogonal matrix)
            
            // Fill scattering matrix with values from Q
            for (int i = 0; i < FDNorder; ++i)
                for (int j = 0; j < FDNorder; ++j)
                    A[i][j] = Q (i, j);
            
            std::cout << "A^T * A = I (should be identity matrix within machine precision (|non-diagonal value| < 1e-15))" << std::endl;
            std::cout << Q.transpose() * Q << std::endl;
            
            break;
        }
    }
}

void FDN::printScatteringMatrix()
{
    for (int i = 0; i < A.size(); ++i)
    {
        for (int j = 0; j < A[i].size(); ++j)
            std::cout << A[i][j] << "  ";
        std::cout << std::endl;
    }
}

void FDN::changeFDNorder (int order, MatrixType matType)
{
    eqCombs.clear();
    FDNorder = order;
    constructor (matType);
    initialise (fs);
    printScatteringMatrix();
}
