close all
clear

for bl = 1:20
    eval(sprintf('load("mu_%d_2");',bl));
    eval(sprintf('norm_mu_%d = normalizedMus(mu_%d_2);',bl,bl));
end

[num_rows,~] = size(mu_1_2);

for row = 1:num_rows
    figure(row);
    hold on;
%     for bl = 10:20
%         eval(sprintf('plot(norm_mu_%d,mu_normal_%d(row,:));',bl,bl));
%     end
    for bl = 1:20
        eval(sprintf('plot(norm_mu_%d,mu_%d_2(row,:));',bl,bl));
    end
    xlabel('Revealed Bits (%)');
    ylabel('Probability (%)');
end