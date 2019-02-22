% Created by Dakota Flanary
function [p_e] = probability_erasure(snr,G,epsilon)
%probability_erasure calculates the probability of erasure for a given snr
%G and epsilon values
%   Calculates f(x) or the gaussian distribution of a transmitted bit, then
%   it calculates the probability of a bit given an x value, then it
%   calculates the entropy of x from which it can calculate the mutual
%   information as a function of x.  With the mutual information it
%   calculates the region of integration for integrating f(x) to find the
%   probability of erasure where the region of integration is the region
%   that the mutual information is less than epsilon
b = [1 -1];
%% Functions
sigmaSquared = 1/snr; % sigma squared is 1/signal to noise ration

fun_x = @(x) (((1/(sqrt(2*pi*sigmaSquared)))*exp((-1/(2*sigmaSquared))*(x-(G*b(1))).^2))+...
    (1/(sqrt(2*pi*sigmaSquared)))*exp((-1/(2*sigmaSquared))*(x-(G*b(2))).^2))...
    .*.5;
    % f(x) or the probability density function of what is received based on
    % the value of x

p_b_x = @(x,n) (1/(sqrt(2*pi*sigmaSquared)))*exp((-1/(2*sigmaSquared))*(x-(G*b(n))).^2)...
    ./(2*fun_x(x)); % pr(b|x) probability that bit n was transmitted given an x value

H = @(x) entropy_dakota(p_b_x(x,1)) + entropy_dakota(p_b_x(x,2)); % entropy on the bit level given an x value

I = @(x) 1-H(x); % mutual information is 1 - the entropy

%%% mdrstuff

foo = @(x) I(x) - epsilon; % foo is used to find where the mutual information crosses epsilon

if isnan(foo(0)) % meaning if the mutual information never goes below epsilon
    p_e = 0; % the probability of erasure is 0
else
    x1 = fzero(foo,0); % finds where I(x) equals epsilon
    if isnan(x1) % if the mutual information never equals epsilon
        p_e = 1; % the probability of earsure is 1
    else
        if x1 < 0 % if x1 is negative
            x2 = -x1; %x2 is the positive version of x1
        else
            x2 = x1; % x2 is the positive value
            x1 = -x1; % x1 becomes the negative limit
        end
        p_e = integral(fun_x,x1,x2); % integrate f(x) from x1 to x2 to calculate
                                    % the probability of erasure
    end
    % toc;
end

%% graphs
%
% figure()
% fplot(fun_x);
% % xlim([-.02,.02]);
% ylim([-1,1]);
% figure()
% fplot(p_e1);
% xlim([-.02,.02]);
% ylim([-1,1]);
% figure();
% fplot(e_1);
% xlim([-10*G,10*G]);
% figure();
% fplot(H);
% xlim([-50*G,50*G]);
% figure();
% fplot(I);
% xlim([-50*G,50*G]);
end

function [h] = entropy_dakota(p)
%entropy_dakota calculates the entropy
%   if p = 0 then the entropy is 0 else it is -p * log2(p)
if p == 0
    h = 0;
else
    h = -p .* log2(p);
end
end