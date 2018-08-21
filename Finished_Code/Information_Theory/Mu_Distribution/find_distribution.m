load('pr_harrison.mat');
load('pr_smalley.mat');

threshold = .99;
gc = good_carriers(pr_harrison, threshold);
for i = 5:5
    block_length = 2^i;
    eval(sprintf( ...
        'mu_%d_%d = bl_distribution_avg(gc,pr_smalley,block_length);'...
        , block_length,2));
    eval(sprintf('save("mu_%d_%d");',block_length,2));
end
