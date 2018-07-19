
function [true] = RMWeightHierM(r1,r2,m1,m2)
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
for m = m1 : m2
    figure();
    for r = r1 : r2
        if r < m
            
            n = 2^m;
            k = 0; %%initialize k = 0
            for i = 0 : r
                k = k + nchoosek(m,i);
            end
            rate = k/n;
            
            alphaValues = zeros(1,m);
            for num = 1 : m
                alphaValues(1, num) = 2^(num-1);
            end
            finalValues = n;
            for numCombos = 1:r
                combos = nchoosek(alphaValues(1,:), numCombos);
                [rows columns] = size(combos);
                for index = 1 : rows
                    comboSum = sum(combos(index,:));
                    finalValues = [finalValues 2^m-comboSum];
                end
            end
            weights = zeros(1, k);
            weights = sort(finalValues);
            
            weightsIndex = 1;
            graphWeights = zeros(1,n+1);
            for index = 1 : n + 1
                if index <= weights(1,weightsIndex)
                    graphWeights(1,index) = k - weightsIndex + 1;
                elseif index > weights(1, weightsIndex)
                    weightsIndex = weightsIndex + 1;
                    graphWeights(1,index) = k - weightsIndex + 1;
                end
            end
            
            graphIndexes = zeros(1,n+1);
            for j = 0 : n
                graphIndexes(1, j+1) = j;
            end
            
            
            
            hold on;
            plot(graphIndexes,graphWeights,'DisplayName',['RM(' num2str(r) ',' num2str(m) '), k = ' num2str(k) ', rate = ' num2str(round(rate,2))]);
            grid on;
            title({['\fontsize{12}Security Curve for RM:  m = ' num2str(m) ''] ; ...
                ['\fontsize{11}n = ' num2str(n)]});
            ylabel('Equivocation (bits)');
            xlabel ('Revealed Bits (\mu)');
            legend;
        end
        
    end
    hold off;
end

end



