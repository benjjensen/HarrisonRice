#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <math.h>
#include <ctime>
#include <cstdio>
#include <omp.h>
#include <set>

static int count = 0;
static int count2 = 0;
/*
*get_values Finds the probability of k carriers being good
*   Receives an array of probabilities and parameters n and k, n being the
*   block length and k being the number of revealed bits.  It then goes
*   through all of the n choose k combinations and calculates the
*   probability of that n and k occuring
*/
void get_values(double *start, const int n, int available_cores, double pr[n+1])
{

  const int bl = n/2;
  if (n == 1) // base case, create a pmf from one independent variable
  {
    pr[0] = 1 - *start;
    pr[1] = *start;
  }
  else // create a pmf from two different pmfs
  {
    double *nstart = start + bl; // pointer to the first starting value of temp_b
    double *temp_a = new double[bl+1]; //temporary array
    double *temp_b = new double[bl+1]; //temporary array
    get_values(start,bl,available_cores,temp_a);
    get_values(nstart,bl,available_cores,temp_b);

    for(int i = 0; i < n+1; i++)
    {
      pr[i] = 0;
    }

    for(long i = 0; i < bl+1; i++)
    {
      for(long j = 0; j < bl+1; j++)
      {
        long k = i + j;
        pr[k] += temp_a[i] * temp_b[j];
      }
    }
    delete[] temp_a;
    delete[] temp_b;
  }

}


/*
*   Converts a .txt file into a 2D array
*/
void make_arr(std::string file, double arr[71][64])
{
  std::ifstream infile(file);
  int row = 0;
  int col = 0;
  while(infile)
  {
    std::string s;
    if(!getline(infile,s))
    {
      break;
    }
    std::istringstream ss(s);
    while(ss)
    {
      std::string st;
      if(!getline(ss,st,','))
      {
        break;
      }
      arr[row][col] = stof(st);
      col++;
    }
    row++;
    col = 0;
  }
  // duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
  // std::cout << "make_arr takes " << duration << " seconds\n";
}
/**
*good_carriers
*Finds the indices where the probability is greater than the threshold
**/
std::vector< std::vector<int> > good_carriers(double arr[71][64], double pr_threshold)
{
  std::vector< std::vector<int> > indices;
  for (int i = 0; i < 71; i++)
  {
    int index = 0;
    for(int j = 0; j < 64; j++)
    {
      if(arr[i][j] >= pr_threshold)
      {
        if(index == 0)
        {
          indices.push_back(std::vector<int> ());
        }
        indices[i].push_back (j);
        index++;
      }
    }
  }
  return indices;
}

/*
*get_groups Gets the potential carrier groups based on blocklength
*   Given a certain block length and a set of carriers it finds every
*   combination of carriers consisting of only adjacent carriers.  Goes until
*   the last carrier in a group is the last good carrier. The group size is bl.
*/
std::vector< std::vector<double> > get_groups(std::vector<double> *pr_mat, int bl)
{
  int i = 0;
  int row = -1;
  std::vector< std::vector<double> > gg;
  do
  {
    row++;
    for(int j = 0; j < bl; j++)
    {
      if(j == 0)
      {
        gg.push_back(std::vector<double> ());
      }
      gg[row].push_back(pr_mat[0][i]);
      if(i == pr_mat->size() - 1)
      {
        i = 0;
      }
      else
      {
        i++;
      }
    }
  } while(i != 0);
  return gg;
}

/*
*bl_get_dist gets the distribution of all of the combos and averages the
*probabilities to get the average distribution
*   input is a matrix of the possible combinations with each row being a
*   different combination of carriers, each combination is passed into the get_values
*   function where the probability for each number of revealed bits is
*   calculated, if there is more than one combination the probabilities are
*   averaged which becomes the output.
*   gg is the vector of groupings
*   bl is the block length of the code
*/
std::vector<double> bl_get_dist(std::vector< std::vector<double> > &gg, const int bl)
{

  std::vector< std::vector<double> > probabilities;
  // #pragma omp parallel for
  for(int i = 0; i < gg.size(); i++)
  {
    double *pr = new double[bl];
    for(int j = 0; j < bl; j++)
    {
      pr[j] = gg[i][j]; //copies a vector in gg to an array
    }
    std::clock_t start;
    double duration;
    start = std::clock();
    double *begin = pr;
    const int nk = bl+1; // the length of the pmf which is the length of the vectors returned by this function
    double *probs = new double[nk]; // the array that will contain the pmf
    get_values(begin,bl,4,probs); // creates the pmf
    std::vector<double> pr_mat;
    for(int j = 0; j < bl+1; j++)
    {
      pr_mat.push_back(probs[j]); // copies the pmf into a vector
    }
    delete[] probs;
    probabilities.push_back(pr_mat); // pushes the pmf into a vector of pmfs
    duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
    // std::cout << "get_values takes " << duration << " seconds\n";
    count2++;
    count++;
  }
  if(gg.size() > 1) // if the size is >1 take the average else return the first vector in the vector
  {
    std::vector<double> prob_arr (bl+1,0.0);
    for(int i = 0; i < probabilities.size(); i++)
    {
      for(int j = 0; j < probabilities[i].size(); j++)
      {
        prob_arr[j] += probabilities[i][j];
      }
    }
    for(int i = 0; i < bl+1; i++)
    {
      prob_arr[i] = prob_arr[i]/probabilities.size();
    }
    return prob_arr;
  }
  else
  {
    return probabilities[0];
  }
}

/*
*bl_distribution_avg Finds the average number of revealed bits as a
*probability distribution
*   Finds the average of number of revealed bits based on the carrier
*   probabilities of the eavesdropper
*/
std::vector< std::vector<double> > bl_distribution_avg(std::vector< std::vector<int> > &gc, double pr_mat[71][64], const int bl)
{
  // std::clock_t start;
  // double duration;
  // start = std::clock();
  std::vector< std::vector<double> > averages;
  int gcs = gc.size(); // determines how many rows there will be in averages
  // #pragma omp parallel for
  for(int i = 0; i < gcs; i++)
  {
    count2 = 0;
    std::vector<double> *pr = new std::vector<double>;
    for(int j = 0; j < gc[i].size(); j++)
    {
      pr->push_back(pr_mat[i][gc[i][j]]); // creates a vector that contains the values in pr_mat that match with the values in gc
      //which are the values in pr_harrison that were greater than the threshold
    }
    std::vector< std::vector<double> > gg = get_groups(pr,bl); //gets the groupings of values of size bl
    delete pr;
    averages.push_back(bl_get_dist(gg,bl));
    std::cout << count2 << '\n';
  }
  // duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
  // std::cout << "bl_distribution_avg takes " << duration << " seconds\n";
  return averages;
}





int main(int argc, char *argv[])
{
  std::clock_t start;
  double duration;
  start = std::clock();

  double pr_harrison [71][64];
  double pr_smalley [71][64];
  make_arr(argv[1],pr_harrison); // convert the txt files to arrays
  make_arr(argv[2],pr_smalley);
  // make_arr("pr_harrison.txt",pr_harrison);
  // make_arr("pr_smalley.txt",pr_smalley);
  double threshold = .9999999; // Sets the threshold for evaluating which indices are good
  std::vector< std::vector<int> > gc = good_carriers(pr_harrison,threshold); // Forms a vector of the indices in pr_harrison who's values are greater than .99
  std::vector< std::vector<double> > *averages = new std::vector< std::vector<double> >;
  std::cout << gc[0].size() << '\n';
  // #pragma omp parallel for
  for(int i = 1; i <= 17; i++) // determines the length of the pmf so when i = 20 the length = 2^20 + 1
  {
    std::clock_t sstart = std::clock();
    const int bl = pow(2,i);
    *averages = bl_distribution_avg(gc,pr_smalley,bl); // averages is a vector where each row is a pmf for a different group of values
    duration = (std::clock() - sstart) / (double) CLOCKS_PER_SEC;
    std::cout << "time for base " << i << " = " << duration << " seconds\n";
    char filenm[9];
    sprintf(filenm,"mu_%d_rel.txt",i);
    std::string filename(filenm);
    std::ofstream outFile(filename); // writes averages to a file
    for(int j = 0; j < averages->size(); j++)
    {
      for(int k = 0; k < averages[0][j].size(); k++)
      {
        if(k != averages[0][j].size() - 1)
        outFile << averages[0][j][k] << ",";
        else
        outFile << averages[0][j][k];
      }
      outFile << "\n";
    }
    std::cout << '\n';
  }
  duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
  std::cout << "total time = " << duration << " seconds\n";
  std::cout << count << "\n";
  return 0;
}
