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
// #include <stdlib.h>
// #include "gmp.h"
// #include "mat.h"
// #include "matrix.h"
// #include "mat.h"

// using namespace std;

/*
*get_values Finds the probability of k carriers being good
*   Receives an array of probabilities and parameters n and k, n being the
*   block length and k being the number of revealed bits.  It then goes
*   through all of the n choose k combinations and calculates the
*   probability of that n and k occuring
*/
// std::vector<double> get_values(double *start, double *end, int n, int available_cores)
void get_values(double *start, const int n, int available_cores, double pr[n+1])
{

  const int bl = n/2;
  if (n == 1)
  {
    // std::vector<double> pr;
    // pr.push_back(1 - *start);
    // pr.push_back(*start);

    pr[0] = 1 - *start;
    pr[1] = *start;
    // std::cout << pr[1] << '\n';
    // return pr;
  }
  else
  {
    std::set<int> kvals;
    const int nk = bl + 1;
    // std::vector<double> pr (n+1);
    // double *nend = start + bl - 1;
    double *nstart = start + bl;
    double *temp_a;
    double *temp_b;
    temp_a = new double[bl+1];
    // for(int i = 0; i < bl+1; i++)
    temp_b = new double[bl+1];
    // std::cout << bl << "\n";

    // double temp_a[nk];
    // double temp_b[nk];
    // std::cout << "foo\n";
    get_values(start,bl,available_cores,temp_a);
    // std::cout << temp_a[0] << " ";
    get_values(nstart,bl,available_cores,temp_b);
    // for(int i = 0; i < bl+1; i++)
    // {
    //   std::cout << temp_a[i] << " ";
    // }
    // std::cout << '\n';
    // for(int i = 0; i < bl+1; i++)
    // {
    //   std::cout << temp_b[i] << " ";
    // }
    // std::cout << '\n';
    // std::vector<double> temp_a = get_values(start,nend,bl,available_cores);
    // std::vector<double> temp_b = get_values(nstart,end,bl,available_cores);
    // std::cout << "Before\n";
    for(int i = 0; i < n+1; i++)
    {
      pr[i] = 0;
    }
    // std::cout << '\n';

    for(int i = 0; i < bl+1; i++)
    {
      for(int j = 0; j < bl+1; j++)
      {
        int k = i + j;
        // std::cout << k;
        pr[k] = temp_a[i] * temp_b[j];
        // std::cout << '\n';
        // if(kvals.count(k) == 0)
        // {
        //   pr[k] = temp_a[i] * temp_b[j];
        //   kvals.insert(k);
        // }
        // else
        // {
        //   pr[k] = pr[k] + temp_a[i] * temp_b[j];
        // }
      }
    }
    // std::cout << "\n";
    // for(int i = 0; i < n+1; i++)
    // {
    //   std::cout << pr[i] << " ";
    // }
    // std:: cout << '\n';
    delete[] temp_a;
    delete[] temp_b;

    // return pr;
  }

}
//     std::vector<double> pr (n+1);
//     // int th = omp_get_num_threads();
//       #pragma omp parallel sections
//       {
//         #pragma omp section
//         {
//           pr_a = get_values(temp_a,bl,first_cores);
//         }
//         #pragma omp section
//         {
//           pr_b = get_values(temp_b,bl,second_cores);
//         }
//       }
//         // #pragma omp parallel for
//         for(int j = 0; j < bl; j++)
//         {
//           int k = i + j;
//           pr[k] = pr[k] + pr_a[i]*pr_b[j];
//         }



// void make_arr(const char *file, double arr[71][64])
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
    // std::cout << '\n';
    std::istringstream ss(s);
    // std::cout << ss.str() << '\n';
    while(ss)
    {
      std::string st;
      if(!getline(ss,st,','))
      {
        break;
      }
      // std::cout << st << '\n';
      arr[row][col] = stof(st);
      // std::cout << stof(st) << '\n';
      col++;
    }
    row++;
    col = 0;
  }
  duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
  // std::cout << "make_arr takes " << duration << " seconds\n";
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
    // std::cout << i << '\n';
    for(int j = 0; j < 64; j++)
    {
      if(arr[i][j] >= pr_threshold)
      {
        if(index == 0)
        {
          indices.push_back(std::vector<int> ());
        }
        // std::cout << j << '\n';
        indices[i].push_back (j);
        index++;
      }
    }
  }
  duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
  // std::cout << "good_carriers takes " << duration << " seconds\n";
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
    // std::cout<< i << '\n';
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
  // std::cout << "get_groups takes " << duration << " seconds\n";
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
      // std::cout << pr[j] << " ";
    }
    std::clock_t start;
    double duration;
    start = std::clock();
    double *begin = pr;
    // double *end = pr + bl - 1;
    // double *probs;
    const int nk = bl+1;
    double *probs;
    probs = new double[nk];
    // std::cout << '\n';
    get_values(begin,bl,4,probs);

    double s = 0;
    // for(int k = 0; k < bl + 1; k++)
    // {
    //   s += probs[k];
    // }
    // std::cout << "\n\n" << s << '\n';
    // for(int j = 0; j < bl+1; j++)
    // {
    //   std::cout << probs[j] << " ";
    // }
    std::vector<double> pr_mat;
    for(int j = 0; j < bl+1; j++)
    {
      pr_mat.push_back(probs[j]);
    }
    delete[] probs;
    probabilities.push_back(pr_mat);
    // probabilities.push_back(std::vector<double>(probs, probs+bl));
    // delete[] probs;
    // probabilities.push_back(get_values(begin,end,bl,4));
    // probabilities.push_back(get_values(gg[i],bl,6));
    duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
    std::cout << "get_values takes " << duration << " seconds\n";
  }
  if(gg.size() > 1)
  {
    // duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
    // std::cout << "bl_get_dist takes " << duration << " seconds\n";
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
    return prob_arr;//sum of probabilities divided by the number of rows
  }
  else
  {
    // duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
    // std::cout << "bl_get_dist takes " << duration << " seconds\n";
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
  if(environment.priority) {
    // Set the priority of this process to the maximum priority.
    int error = setpriority(PRIO_PROCESS, 0, -20);
    if(error) {
        std::cerr << "ERROR: UNABLE TO INCREASE THE PRIORITY OF ACQUIRE" <<
                std::endl;
    }
    else {
        std::cout << "Increased process priority to maximum." << std::endl;
    }
  }
  std::clock_t start;
  double duration;
  start = std::clock();
  std::vector< std::vector<double> > averages;
  int gcs = gc.size();
  #pragma omp parallel for
  for(int i = 0; i < gcs; i++)
  {
    std::vector<double> pr;
    // double pr_cols = gc[i].size();
    // double power = floor(log2(pr_cols));
    // pr_cols = pow(2,power);
    // double pr [pr_cols];

    for(int j = 0; j < gc[i].size(); j++)
    {
    // for(int j = 0; j < pr_cols; j++)
      pr.push_back(pr_mat[i][gc[i][j]]);
      // pr[j] = pr_mat[i][gc[i][j]];
    }
    // double pr_cols = pr.size();
    // double power = floor(log2(pr_cols));
    // pr_cols = pow(2,power);
    // // std::cout << pr_cols << '\n';
    // pr.erase(pr.begin()+pr_cols,pr.end());
    // std::cout << pr.size() << '\n';
    std::vector< std::vector<double> > gg = get_groups(pr,bl);
    // std::cout << gg.size() << "\n\n";
    averages.push_back(bl_get_dist(gg,bl));
  }
  duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
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
  // make_arr(argv[1],pr_harrison);
  // make_arr(argv[2],pr_smalley);
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
    std::ofstream outFile("mu_18.txt");
    for(int j = 0; j < averages.size(); j++)
    {
      for(int k = 0; k < averages[j].size(); k++)
      {
        if(k != averages[j].size() - 1)
        outFile << averages[j][k] << ",";
        else
        outFile << averages[j][k];
      }
      outFile << "\n";
    }
    std::cout << '\n';
  }

  duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
  std::cout << "total time = " << duration << " seconds\n";
  return 0;
}
