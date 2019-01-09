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


/*
*get_values Finds the probability of k carriers being good
*   Receives an array of probabilities and parameters n and k, n being the
*   block length and k being the number of revealed bits.  It then goes
*   through all of the n choose k combinations and calculates the
*   probability of that n and k occuring
*/
void get_values(double *start, const int n, int available_cores, double pr[n+1])
{

  const int bl = n/2; // size of arrays for this level
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
    get_values(start,bl,available_cores,temp_a); // find the pmf of the first half of the data
    get_values(nstart,bl,available_cores,temp_b); // find the pmf of the second half of the data
    for(int i = 0; i < n+1; i++)
    {
      pr[i] = 0; // initialize the array to 0
    }
    for(int i = 0; i < bl+1; i++)
    {
      for(int j = 0; j < bl+1; j++)
      {
        int k = i + j;
        pr[k] += temp_a[i] * temp_b[j]; // adds the probability of all the combinations of i and j that equal k
      }
    }
  }
}

void make_arr(std::string file, double arr[71][64])
{
  std::clock_t start;
  double duration;
  start = std::clock();
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
  duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
}
/**
*good_carriers
*Finds the indices where the probability is greater than the threshold
**/
std::vector< std::vector<int> > good_carriers(double arr[71][64], double pr_threshold)
{
  std::clock_t start;
  double duration;
  start = std::clock();
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
  duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
  return indices;
}

/*
*get_groups Gets the potential carrier groups based on blocklength
*   Given a certain block length and a set of carriers it finds every
*   combination of carriers to use all of the carriers and the given block
*   length and each group consists of only adjacent carriers.
*/
std::vector< std::vector<double> > get_groups(std::vector<double> &pr_mat, int bl)
{
  std::clock_t start;
  double duration;
  start = std::clock();
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
      gg[row].push_back(pr_mat[i]);
      if(i == pr_mat.size() - 1)
      {
        i = 0;
      }
      else
      {
        i++;
      }
    }
  } while(i != 0);
  duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
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
*/
std::vector<double> bl_get_dist(std::vector< std::vector<double> > &gg, const int bl)
{

  std::vector< std::vector<double> > probabilities;
  for(int i = 0; i < gg.size(); i++)
  {
    double pr [bl];
    for(int j = 0; j < bl; j++)
    {
      pr[j] = gg[i][j];
    }
    std::clock_t start;
    double duration;
    start = std::clock();
    double *begin = pr;
    const int nk = bl+1;
    double probs [nk];
    get_values(begin,bl,4,probs);
    double s = 0;
    std::vector<double> pr_mat;
    for(int j = 0; j < bl+1; j++)
    {
      pr_mat.push_back(probs[j]);
    }
    probabilities.push_back(pr_mat);
    duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
    std::cout << "get_values takes " << duration << " seconds\n";
  }
  if(gg.size() > 1)
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
  std::clock_t start;
  double duration;
  start = std::clock();
  std::vector< std::vector<double> > averages;
  for(int i = 0; i < gc.size(); i++)
  {
    std::vector<double> pr;
    for(int j = 0; j < gc[i].size(); j++)
    {
      pr.push_back(pr_mat[i][gc[i][j]]);
    }
    std::vector< std::vector<double> > gg = get_groups(pr,bl);
    averages.push_back(bl_get_dist(gg,bl));
  }
  duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
  return averages;
}





int main(int argc, char *argv[])
{
  std::clock_t start;
  double duration;
  start = std::clock();

  double pr_harrison [71][64];
  double pr_smalley [71][64];
  make_arr("pr_harrison.txt",pr_harrison);
  make_arr("pr_smalley.txt",pr_smalley);
  double threshold = .99;
  std::vector< std::vector<int> > gc = good_carriers(pr_harrison,threshold);
  std::vector< std::vector<double> > averages;
  // #pragma omp parallel for
  for(int i = 18; i <= 18; i++)
  {
    std::clock_t sstart = std::clock();
    const int bl = pow(2,i);
    averages = bl_distribution_avg(gc,pr_smalley,bl);
    duration = (std::clock() - sstart) / (double) CLOCKS_PER_SEC;
    std::cout << "time for base " << i << " = " << duration << " seconds\n";
  }

  duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
  std::cout << "total time = " << duration << " seconds\n";
  return 0;
}
