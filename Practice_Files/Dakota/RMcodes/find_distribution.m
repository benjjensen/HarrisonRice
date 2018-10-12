load('pr_harrison.mat');
load('pr_smalley.mat');

gc = good_carriers(pr_harrison);
block_length = 4;
mu_4 = bl_distribution_avg(gc,pr_smalley,block_length);
