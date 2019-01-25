function [p_e] = probability_erasure(snr,G,epsilon)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
b = [1 -1];
% tic;
% sigmaSquared = 16;
% G = 1;
% epsilon = .01;
%% Functions
sigmaSquared = 1/snr;
fun_x = @(x) (((1/(sqrt(2*pi*sigmaSquared)))*exp((-1/(2*sigmaSquared))*(x-(G*b(1))).^2))+...
    (1/(sqrt(2*pi*sigmaSquared)))*exp((-1/(2*sigmaSquared))*(x-(G*b(2))).^2))...
    .*.5;

p_b_x = @(x,n) (1/(sqrt(2*pi*sigmaSquared)))*exp((-1/(2*sigmaSquared))*(x-(G*b(n))).^2)...
    ./(2*fun_x(x));

H = @(x) entropy_dakota(p_b_x(x,1)) + entropy_dakota(p_b_x(x,2));

I = @(x) 1-H(x);

%%% mdrstuff

foo = @(x) I(x) - epsilon;
% x1 = fzero(foo,starting_point);

% look up fzero, and find out how to make it find TWO zero crossings.

% I think this will work if x2 = -x1.
if isnan(foo(0))
    p_e = 0;
else
    x1 = fzero(foo,0);
    if isnan(x1)
        p_e = 1;
    else
        if x1 < 0
            x2 = -x1;
        else
            x2 = x1;
            x1 = -x1;
        end
        
        % integrate from x1 to x2
        
        %%% end mdrstuff
        
        % e_1 = @(x) (I(x) < epsilon);
        
        % p_e1 = @(x) e_1(x) .* fun_x(x);
        % q = integral(fun_x,-inf,inf)
        p_e = integral(fun_x,x1,x2);
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