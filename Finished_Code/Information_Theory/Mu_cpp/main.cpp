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
void get_values(double *start, double *end, int n, int available_cores, double pr[n+1])
{

  int bl = n/2;
  if (n == 1)
  {
    // std::vector<double> pr;
    // pr.push_back(1 - *start);
    // pr.push_back(*start);
    pr[0] = 1 - *start;
    pr[1] = *start;
    // return pr;
    // std::cout << *start << " ";
  }
  else
  {
    // std::vector<double> pr (n+1);
    double *nend = start + bl - 1;
    double *nstart = start + bl;
    double temp_a[bl+1];
    // for(int i = 0; i < bl+1; i++)
    double temp_b[bl+1];
    get_values(start,nend,bl,available_cores,temp_a);
    // std::cout << temp_a[0] << " ";
    get_values(nstart,end,bl,available_cores,temp_b);
    // std::vector<double> temp_a = get_values(start,nend,bl,available_cores);
    // std::vector<double> temp_b = get_values(nstart,end,bl,available_cores);
    for(int i = 0; i <= bl; i++)
    {
      for(int j = 0; j <= bl; j++)
      {
        int k = i + j;
        pr[k] = pr[k] + temp_a[i] * temp_b[j];
      }
    }
    // return pr;
  }

}
// std::vector<double> get_values(std::vector<double> &pr_mat, int n, int available_cores)
// {
//
//   int bl = n/2;
//   if(n == 1)
//   {
//     std::vector<double> pr;
//     pr.push_back(1 - pr_mat[0]);
//     pr.push_back(pr_mat[0]);
//     return pr;
//   }
//   else
//   {
//     std::vector<double> pr (n+1);
//     // int th = omp_get_num_threads();
//     if (available_cores > 0)
//     {
//       // int first_cores = (available_cores - 1)/2 + (available_cores-1)%2;
//       // int second_cores = (available_cores - 1)/2;
//       int first_cores = 3;
//       int second_cores = 3;
//       std::vector<double>::iterator first = pr.begin();
//       std::vector<double>::iterator last = pr.begin() + bl;
//       std::vector<double> temp_a(first,last);
//       first = pr.begin() + bl + 1;
//       last = pr.end();
//       // temp_a.insert(it,pr.begin(),pr.begin()+bl);
//       std::vector<double> temp_b(first,last);
//       // it = temp_b.begin();
//       // temp_b.insert(it,pr.begin()+bl+1,pr.end());
//       std::vector<double> pr_a;
//       std::vector<double> pr_b;
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
//       for(int i = 0; i < bl; i++)
//       {
//         // #pragma omp parallel for
//         for(int j = 0; j < bl; j++)
//         {
//           int k = i + j;
//           pr[k] = pr[k] + pr_a[i]*pr_b[j];
//         }
//       }
//     }
//     else
//     {
//       std::vector<double>::iterator first = pr.begin();
//       std::vector<double>::iterator last = pr.begin() + bl;
//       std::vector<double> temp_a(first,last);
//       first = pr.begin() + bl + 1;
//       last = pr.end();
//       // temp_a.insert(it,pr.begin(),pr.begin()+bl);
//       std::vector<double> temp_b(first,last);
//       // it = temp_b.begin();
//       // temp_b.insert(it,pr.begin()+bl+1,pr.end());
//       std::vector<double> pr_a = get_values(temp_a,bl,0);
//       std::vector<double> pr_b = get_values(temp_b,bl,0);
//
//       for(int i = 0; i < bl; i++)
//       {
//         for(int j = 0; j < bl; j++)
//         {
//           int k = i + j;
//           pr[k] = pr[k] + pr_a[i]*pr_b[j];
//         }
//       }
//     }
//     return pr;
//   }

// }


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
std::vector<double> bl_get_dist(std::vector< std::vector<double> > &gg, int bl)
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
    double *end = pr + bl - 1;
    double probs[bl+1];
    get_values(begin,end,bl,4,probs);
    // for(int j = 0; j < bl+1; j++)
    // {
    //   std::cout << probs[j] << " ";
    // }
    probabilities.push_back(std::vector<double>(probs, probs + sizeof probs / sizeof probs));
    // probabilities.push_back(get_values(begin,end,bl,4));
    // probabilities.push_back(get_values(gg[i],bl,6));
    duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
    std::cout << "get_values takes " << duration << " seconds\n";
  }
  if(gg.size() > 1)
  {
    // duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
    // std::cout << "bl_get_dist takes " << duration << " seconds\n";
    std::vector<double> probs (probabilities.size());
    for(int i = 0; i < probabilities.size(); i++)
    {
      for(int j = 0; j < probabilities[i].size(); j++)
      probs[j] += probabilities[i][j]/(bl+1);
    }
    return probs;//sum of probabilities divided by the number of rows
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
std::vector< std::vector<double> > bl_distribution_avg(std::vector< std::vector<int> > &gc, double pr_mat[71][64], int bl)
{
  std::clock_t start;
  double duration;
  start = std::clock();
  std::vector< std::vector<double> > averages;
  for(int i = 0; i < gc.size(); i++)
  {
    std::vector<double> pr;
    // double pr_cols = gc[i].size();
    // double power = floor(log2(pr_cols));
    // pr_cols = pow(2,power);
    // double pr [pr_cols];
    for(int j = 0; j < gc[i].size(); j++)
    // for(int j = 0; j < pr_cols; j++)
    {
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

  // int p[3] = {5,2,1};
  // int *pp = p;
  // int *pe = pp + 3;
  // std::cout << *pp << " ";
  // pp[1] = 4;
  // std::cout << pp[1] << " ";
  // pp++;
  // std::cout << *pp << '\n';
  // *pp = 3;
  // std::cout << *pe << '\n';
  // std::cout << argv[1] << '\n';
  std::clock_t start;
  double duration;
  start = std::clock();
  // std::string s = "2.1234";
  // std::cout << stof(s) << '\n';
  double pr_harrison [71][64];
  double pr_smalley [71][64];
  // make_arr(argv[1],pr_harrison);
  // make_arr(argv[2],pr_smalley);
  make_arr("pr_harrison.txt",pr_harrison);
  make_arr("pr_smalley.txt",pr_smalley);
  // std::cout << "\n\npr_harrison\n\n";
  //
  // for(int i = 0; i < 71; i++)
  // {
  //   for(int j = 0; j < 64; j++)
  //   {
  //     std::cout << pr_harrison[i][j] << ",";
  //   }
  //   std::cout << '\n';
  // }
  double threshold = .99;
  std::vector< std::vector<int> > gc = good_carriers(pr_harrison,threshold);
  std::vector< std::vector<double> > averages;
  // #pragma omp parallel for
  for(int i = 0; i <= 15; i++)
  {
    std::clock_t sstart = std::clock();
    averages = bl_distribution_avg(gc,pr_smalley,pow(2,i));
    duration = (std::clock() - sstart) / (double) CLOCKS_PER_SEC;
    std::cout << "time for base " << i << " = " << duration << " seconds\n";
    for(int j = 0; j < averages[0].size(); j++)
    {
      std::cout << averages[0][j];
    }
  }
  // std::cout << averages.size() << '\n';
  // std::cout << averages[0].size() << '\n';
  // std::cout << averages[19][0].size() << '\n';
  duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
  std::cout << "total time = " << duration << " seconds\n";
  return 0;
}
