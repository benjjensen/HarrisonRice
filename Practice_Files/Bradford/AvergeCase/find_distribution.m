load('pr_harrison.mat');
load('pr_smalley.mat');

threshold = .99;
gc = good_carriers(pr_harrison, threshold);
tic;
for i = 0:20
    disp(i);
    block_length = 2^i;
    eval(sprintf( ...
        'mu_%d = bl_distribution_avg(gc,pr_smalley,block_length);'...
        , block_length));
    eval(sprintf('save("mu_%d.mat","mu_%d");',block_length,block_length));
end
toc;
