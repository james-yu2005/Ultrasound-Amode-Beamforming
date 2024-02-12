#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>
using namespace std;
float **createDataMatrix(int numElement, int numSample)
{
    // Create a 2D data matrix of size numElement and numSample
    float **RFData = new float*[numElement]; // Declaring a double pointer for a new array by referencing the memory space
    for (int i=0; i<numElement; i++)
    {
        RFData [i]= new float[numSample]; // Creating a horizontal row with the numSample in RFData array

        for (int k=0; k<numSample; k++)
        {
            RFData[i][k]=0;// Creating a two dimensional Array
        }
    }
    return RFData;// returning the array
}
int loadRFData(float **RFData, const char *fileName, int numElement, int numSample)
{
    // Open the text file fileName, read the data and store into RFData
    // You can use the getline() command to extract the data lines from the txt files
    ifstream input(fileName); // command to opent he file
    if (!input)
    {
    cerr<< "The file doesn't open" << endl; // error to be displayed incase the file doesn't open
    return -1;
    }
    for(int i=0; i<numElement; i++) // reading information from the file
    { 
        for (int j=0; j<numSample; j++)
        {
           char temp[30]; // creating a character array of 30 elements
           input.getline(temp,30); // reading data from the input line
           RFData[i][j]=atof(temp); // converting the RFData to float
        }
    }
    input.close (); // close the file which is opened
    return 0;
}
// Create an array containing the depth location (in z-direction) for each pixel on the scanline
float *genScanlineLocation(int &numPixel)
{
    float imagingDepth;
    cout << "Enter the desired imaging depth (in meters) (should be positive): "; // asking the user to input the desired depth as float
    cin >> imagingDepth;
    cout << "Enter the number of pixels on the scanline: ";// asking the user to input the  number of pixels as a float
    cin >> numPixel;
    float* scanlineLocation = new float[numPixel]; // creating a 1D array for numPixel
    for (int i=0; i<numPixel; i++)
    {
        scanlineLocation [i]= i* (imagingDepth/(numPixel-1.0)); // The formula to calculate scanlineLocation
    }
    return scanlineLocation;
}
// Create an array containing the element location (in x-direction) of the ultrasound transducer
float *genElementLocation(int numElement, float PITCH)
{
   float* eleLocation = new float [numElement]; // creating an array for numELment
   for (int n=0; n<numElement; n++)
   {
    eleLocation[n]=(n-((numElement-1.0)/2.0))*PITCH; // calculating the value of eleLocation
   }
    return eleLocation;
}
// Allocate memory to store the beamformed scanline
float *createScanline(int numPixel)
{
    float* scanline = new float[numPixel]; //creating a new array called numPixel while refering the memory
    return scanline;
}
// Beamform the A-mode scanline
void beamform(float *scanline, float **realRFData, float **imagRFData, float *scanlinePosition, float *elementPosition, int numElement, int numSample, int numPixel, float FS, float SoS)
{
    //creating different arrays for different elemnents 
    float* tforward = new float[numPixel]; 
    float** tbackward = new float *[numPixel];
    float** ttotal = new float*[numPixel];
    int** s = new int*[numPixel];
    float* Preal = new float [numPixel];
    float* Pimag = new float [numPixel];
    float* mag = new float [numPixel];
    for (int i=0 ; i < numPixel; i++)
    {
        tforward[i]= (scanlinePosition [i])/ SoS; //Calculations for tforward
        tbackward[i] = new float[numElement];// creating a new array for tbackward
        ttotal[i]= new float [numElement];//creating a new array for ttotal
        s[i]= new int [numElement];// creating an int array for sample size
        Preal[i] = 0;
        Pimag[i] = 0;
        for (int k=0; k<numElement; k++ )
        {
            //creating a 2-D array
            tbackward[i][k]= sqrt(pow(scanlinePosition[i], 2) + pow(elementPosition[k],2))/ SoS;// calculating for tbackward
            ttotal [i][k]= tforward[i]+ tbackward [i][k];//calculating for ttotal
            s[i][k]= floor(ttotal [i][k]*FS);// calculating for sample 's'
            Preal [i] += realRFData [k] [s[i][k]];// calculating for Preal
            Pimag [i]+= imagRFData [k] [s[i][k]];//calculating for Pimag
        }
    }  
    for (int i=0; i<numPixel; i++)
    {   //calculate for magnitude
        mag[i] = sqrt(pow(Preal[i],2)+ (pow(Pimag[i],2))); 
        scanline[i]= mag[i];
    }
}
// Write the scanline to a csv file
int outputScanline(const char *fileName, float *scanlinePosition, float *scanline, int numPixel)
{ 
    ofstream outputFile(fileName); // open the file
    if (!outputFile) 
    {
        cerr<< "The file doesn't open" << endl;// displa error if the file doesn't open
        return -1;
    }
    for (int i = 0; i < numPixel; i++) 
    {
        outputFile << scanlinePosition[i] << ',' << scanline[i] <<endl; //Display this way final
    }
    outputFile.close();// close the output file
    return 0;
}
// Destroy all the allocated memory
void destroyAllArrays(float *scanline, float **realRFData, float **imagRFData, float *scanlinePosition, float *elementPosition, int numElement, int numSample, int numPixel)
{
    delete[] scanline; // delete the scanline array
    for (int i = 0; i < numElement; i++) 
    {
        delete[] realRFData[i]; // delete real RFData all elements in the array
        delete[] imagRFData[i];// delete imagRFData all elements in the array
    }
    delete[] realRFData;// delete realRFData array
    delete[] imagRFData;// delete imagRFData
    delete[] scanlinePosition; // delete scanlinePosition
    delete[] elementPosition;// delete elementPosition
}
