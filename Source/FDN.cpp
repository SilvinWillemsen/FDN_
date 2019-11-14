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
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    b.resize (Global::FDNorder, 1.0);
    c.resize (Global::FDNorder, 1.0);
    
    std::vector<std::vector<float>> tmpMatrix (4, std::vector<float> (4, -1));
    for (int i = 0; i < 4; ++i)
        tmpMatrix[i][i] = 1;
    
    A.resize (Global::FDNorder, std::vector<float> (Global::FDNorder, 0));
    for (int i = 0; i < Global::FDNorder; ++i)
    {
        for (int j = 0; j < Global::FDNorder; ++j)
        {
            if (i / 4 == j / 4)
                A[i][j] = 0.25 * tmpMatrix[i % 4][j % 4];
            else
                A[i][j] = -0.25 * tmpMatrix[i % 4][j % 4];
//            std::cout << A[i][j] << " ";
//            if (j % 4 == 3)
//                std::cout << "   ";
        }
//        std::cout << std::endl;
//        if (i % 4 == 3)
//            std::cout << std::endl;
    }
}

FDN::~FDN()
{
}

void FDN::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (14.0f);
    g.drawText ("FDN", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void FDN::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

float FDN::calculate (float input)
{
    output = 0;
    std::vector<float> intermediateOutput (Global::FDNorder, 0);
    
    for (int i = 0; i < Global::FDNorder; ++i)
    {
        intermediateOutput[i] = c[i] * eqCombs[i]->filter (b[i] * input);
        for (int j = 0; j < Global::FDNorder; ++j)
        {
            eqCombs[j]->addScatOutput (A[j][i] * intermediateOutput[i]);
        }

        output += intermediateOutput[i];
    // calculate using output of scattering matrix
    // eqComb->filter
    
    
    // output of eqComb goes out and back into scattering matrix
        
    }
    for (int i = 0; i < Global::FDNorder; ++i)
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
    
    std::vector<float> RT (Global::numOctaveBands, Global::RT);
    
    std::vector<std::vector<float>> gainDB (Global::FDNorder, std::vector<float> (Global::numOctaveBands, 0));
    
    
    std::vector<std::vector<float>> numsopt (3, std::vector<float> (Global::numOctaveBands + 1, 0));
    std::vector<std::vector<float>> densopt (3, std::vector<float> (Global::numOctaveBands + 1, 0));
    
    std::vector<int> dLen (Global::FDNorder, 0);
    getAttenuation (RT, dLen, gainDB);
    
    for (int i = 0; i < Global::FDNorder; ++i)
    {
        eqCombs.add (new EQComb (dLen[i]));
    }
    
    for (int i = 0; i < Global::FDNorder; ++i)
    {
        aceq (gainDB[i], numsopt, densopt);
        for (int j = 0; j < Global::numOctaveBands + 1; ++j)
            eqCombs[i]->setFilter (j, std::vector<float> {
                numsopt[0][j], numsopt[1][j], numsopt[2][j],
                densopt[0][j], densopt[1][j], densopt[2][j]
            });
    }
    std::vector<float> vals;
    
    for (int i = 0; i < Global::FDNorder; ++i)
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
}

std::vector<float> FDN::pareq (float G, float GB, float w0, float bw)
{
    float beta;
    if (G == 1)
        beta = tan(bw / 2.0); // To avoid division by zero, when G = 1
    else
        beta = sqrt(abs(GB * GB - 1) / abs(G*G - GB*GB)) * tan(bw / 2.0);
    
    //num[0]
    float num1 = (1.0 + G * beta) / (1.0 + beta);
    //num[1]
    float num2 = (-2.0 * cos(w0)) / (1.0 + beta);
    //num[2]
    float num3 = (1.0 - G * beta)/(1.0 + beta);
    
    //den[0]
    float den1 = 1.0;
    //den[1]
    float den2 = (-2.0 * cos(w0)) / (1.0 + beta);
    //den[2]
    float den3 = (1.0 - beta) / (1.0 + beta);
    
    return std::vector<float> {num1, num2, num3, den1, den2, den3};
}

void FDN::interactionMatrix (float* G, float gw, float *wg, float *wc, float *bw, std::vector<std::vector<float>>& leak)
{
    std::vector<std::complex<float>> dummyVect (Global::numOctaveBands, 0);
    //leak = zeros(M, N);// Initialize interaction matrix
    std::vector<float> Gdb (Global::numOctaveBands, 0);
    std::vector<float> Gdbw (Global::numOctaveBands, 0);
    std::vector<float> Gw (Global::numOctaveBands, 0);
    float sumG = 0;
    std::vector<float> Gain (Global::numOctaveBands, 0);
    
    std::complex<float> i (0, -1);
    
    //    std::vector<std::vector<float>> coeffs (6, dummyVect);
    
    for (int m = 0; m < Global::numOctaveBands; m++)
    {
        Gdb[m] = 20 * log10 (G[m]); // Convert linear gain factor to dB
        Gdbw[m] = gw * Gdb[m]; // dB gain at bandwidth
        Gw[m] = pow(10, (Gdbw[m] / 20)); // Convert to linear gain factors
        sumG = sumG + Gdb[m];
    }
    
    //Estimate leak factors of peak / notch filters
    std::vector<float> coeffs (6, 0); // for one frequency band
    std::vector<std::complex<float>> complexCoeffs (6, 0); // for one frequency band
    
    std::complex<float> H;
    if (abs(sumG) != 0)
    {
        for (int m = 0; m < Global::numOctaveBands; m++)
        {// Filters
            coeffs = pareq (G[m], Gw[m], wg[m], bw[m]);//Parametric EQ filter
            for (int i = 0; i < coeffs.size(); ++i)
                complexCoeffs[i].real (coeffs[i]);
            
            for (int k = 0; k < Global::numDesignFreqs; k++) // wc is vector of central frqs, so the sized is 10 - most liekly, in this case at least
            {
                H = (complexCoeffs[0] + complexCoeffs[1] * exp(-i * wc[k]) + complexCoeffs[2] * exp(-2.0f * i * wc[k])) / (complexCoeffs[3] + complexCoeffs[4] * exp(-i * wc[k]) + complexCoeffs[5] * exp(-2.0f * i * wc[k]));
                leak[m][k] = abs (20.0f * log10 (H) / Gdb[m]);
            }
        }
    }
}

void FDN::aceq (std::vector<float> Gdb, std::vector<std::vector<float>>& numsopt, std::vector<std::vector<float>>& densopt)
{
    std::vector<float> dummyVect (Global::numOctaveBands + 1, 0);
    numsopt.resize (3, dummyVect);
    densopt.resize (3, dummyVect);
    
    int halfOctaveBands = floor (Global::numOctaveBands * 0.5);
    float G0 = (Gdb[halfOctaveBands] + Gdb[halfOctaveBands + 1]) * 0.5f;
    
    for (int i = 0; i < Global::numOctaveBands; i++)
        Gdb[i] = -G0 + Gdb[i];
    
    // Make this dynamic!! (using 125*2^(n-3))
    std::vector<float> fc1 = { 31, 63, 125, 250, 500, 1000, 2000, 4000, 8000, 16000 }; // 16000. / (2. ^ (9:-1 : 0));% Exact log center frequencies for filters
    std::vector<float> fc2 = { 31, 44.2, 63.0, 88.7, 125, 176.8, 250, 353.6, 500, 707.1, 1000, 1414.2, 2000, 2828.4, 4000, 5656.9, 8000, 11313.7, 16000 };
    
    // wg = [.0044 .009 .0178 .0356 .0712 .01425 0.2850 0.5699 1.1398 2.28];
    std::vector<float> wg (Global::numOctaveBands, 0);
    std::vector<float> wc (Global::numDesignFreqs, 0);
    for (int i = 0; i < Global::numOctaveBands; i++)
        wg[i] = 2.0 * float_Pi * fc1[i] / fs; // Command gain frequencies in radians
    
    for (int j = 0; j < Global::numDesignFreqs; j++)
        wc[j] = 2.0 * float_Pi * fc2[j] / fs; // Center frequencies in radians for iterative design with extra points
    
    float gw = 0.3f; //Gain factor at bandwidth(parameter c)
    std::vector<float> bw (Global::numOctaveBands, 0); // EQ filter bandwidths
    
    for (int m = 0; m < Global::numOctaveBands; m++)
        bw[m] = 1.5 * wg[m];
    
    if (Global::numOctaveBands == 10)
    {
        bw[Global::numOctaveBands - 3] = 0.93 * bw[Global::numOctaveBands - 3];
        bw[Global::numOctaveBands - 2] = 0.78 * bw[Global::numOctaveBands - 2];
        bw[Global::numOctaveBands - 1] = 0.76 * bw[Global::numOctaveBands - 1]; // Additional adjustmenst due to asymmetry
    }
    
    std::vector<float> dummyVect2 (Global::numDesignFreqs, 0);
    std::vector<std::vector<float>> leak (Global::numOctaveBands, dummyVect2);
    std::vector<std::vector<float>> leak2 (Global::numOctaveBands, dummyVect2);
    
    const float c = pow(10, 17.0 / 20.0);
    std::vector<float> inG (Global::numOctaveBands, c);
    interactionMatrix (&inG[0], gw, &wg[0], &wc[0], &bw[0], leak); //Estimate leakage b / w bands
    
    std::vector<float> Gdb2 (Global::numDesignFreqs, 0);
    
    for (int k = 0; k < Global::numDesignFreqs; ++k)
        if (k % 2 == 0)
            Gdb2[k] = Gdb[k / 2];
        else
            Gdb2[k] = (Gdb2[k-1] + Gdb2[k+1]) / 2.0; // Interpolate target gains linearly b / w command gains
    
    std::vector<float> Goptdb (Global::numOctaveBands, 0);
    std::vector<float> Gopt (Global::numOctaveBands, 0);
    std::vector<float> G2optdb (Global::numOctaveBands, 0);
    std::vector<float> G2opt (Global::numOctaveBands, 0);
    std::vector<float> G2woptdb (Global::numOctaveBands, 0);
    std::vector<float> G2wopt (Global::numOctaveBands, 0);
    
    //// Linear least squares ////
    Eigen::MatrixXf leakMat (Global::numDesignFreqs, Global::numOctaveBands);
    // transpose matrix
    for (int i = 0; i < Global::numOctaveBands; i++)
        for (int j = 0; j < Global::numDesignFreqs; j++)
            leakMat(j, i) = leak[i][j];
    
    // Linear least squares
    Eigen::VectorXf Gdb2Vect;
    Gdb2Vect = Eigen::VectorXf::Map (&Gdb2[0], Gdb2.size());
    Gdb2Vect = Gdb2Vect.transpose();
    Eigen::VectorXf solution = leakMat.bdcSvd (Eigen::ComputeThinU | Eigen::ComputeThinV).solve(Gdb2Vect);
    
    for (int i = 0; i < Global::numOctaveBands; ++i)
        Goptdb[i] = solution(i);
    
    for (int k = 0; k < Global::numOctaveBands; k++)
        Gopt[k] = pow(10, Goptdb[k] / 20.0); // Convert to linear gain factors
    
    // (Second interaction matrix) Iterate once
    interactionMatrix (&Gopt[0], gw, &wg[0], &wc[0], &bw[0], leak2); // Use previous gains
    
    Eigen::MatrixXf leakMat2 (Global::numDesignFreqs, Global::numOctaveBands);
    // transpose matrix
    for (int i = 0; i < Global::numOctaveBands; i++)
        for (int j = 0; j < Global::numDesignFreqs; j++)
            leakMat2(j, i) = leak2[i][j];
    
    // Linear least squares
    Eigen::VectorXf G2optdbVect;
    G2optdbVect = Eigen::VectorXf::Map (&Gdb2[0], Gdb2.size());
    G2optdbVect = G2optdbVect.transpose();
    Eigen::VectorXf solution2 = leakMat2.bdcSvd (Eigen::ComputeThinU | Eigen::ComputeThinV).solve(G2optdbVect);
    
    for (int i = 0; i < Global::numOctaveBands; ++i)
        G2optdb[i] = solution2(i);
    
    for (int k = 0; k < Global::numOctaveBands; k++)
    {
        G2opt[k] = pow(10, G2optdb[k] / 20.0); // Convert to linear gain factors
        G2woptdb[k] = gw * G2optdb[k];        // Gain at bandwidth wg
        G2wopt[k] = pow(10, G2woptdb[k] / 20.0); //Convert to linear gain factor
    }
    
    // Design filters with optimized gains
    std::vector<float> coeffsFromPareq (6, 0);
    for (int k = 0; k < Global::numOctaveBands; k++)
    {
        coeffsFromPareq = pareq (G2opt[k], G2wopt[k], wg[k], bw[k]); // Design filters
        
        numsopt[0][k] = coeffsFromPareq[0] * pow(10 ,(G0 / 200.0));
        numsopt[1][k] = coeffsFromPareq[1] * pow(10, (G0 / 200.0));
        numsopt[2][k] = coeffsFromPareq[2] * pow(10, (G0 / 200.0));
        densopt[0][k] = coeffsFromPareq[3];
        densopt[1][k] = coeffsFromPareq[4];
        densopt[2][k] = coeffsFromPareq[5];
    }
    
    // High Shelving filter
    float GHdb = Gdb[Global::numOctaveBands - 1];// min(gDB);
    float fH = 20200.f; // 20200;
    float GH = pow(10, GHdb / 20.0); // linear gain
    float wH = 2.0 * float_Pi * fH / fs; // frequency in[rad]
    
    //  coefficients
    float aH0 = sqrt(GH) * tan(wH * 0.5) + 1;
    float aH1 = sqrt(GH) * tan(wH * 0.5) - 1;
    float bH0 = sqrt(GH) * tan(wH * 0.5) + GH;
    float bH1 = sqrt(GH) * tan(wH * 0.5) - GH;
    
    
    numsopt[0][Global::FDNorder] = bH0 / aH0;
    numsopt[1][Global::FDNorder] = bH1 / aH0;
    densopt[0][Global::FDNorder] = aH0 / aH0;
    densopt[1][Global::FDNorder] = aH1 / aH0;
    
}

void FDN::getAttenuation (std::vector<float> RT, std::vector<int>& dLen, std::vector<std::vector<float>>& gainDB)
{
    Random rand;
    
    for (int i = 0; i < Global::FDNorder; ++i)
        dLen[i] = round (Global::minDelayLength + (Global::maxDelayLength - Global::minDelayLength) * rand.nextFloat());
    
    for (int k = 0; k < Global::FDNorder; k++)
    {
        for (int m = 0; m < Global::numOctaveBands; m++)
        {
            gainDB[k][m] = (-60.0 / (RT[m] * fs)) * dLen[k];
        }
    }
}

void FDN::debugPrint (std::vector<std::vector<float>>& vect)
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
