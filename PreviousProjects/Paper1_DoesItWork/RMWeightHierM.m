function [false] = RMWeightHierM(r1,r2,m1,m2)
%   This function finds the RM weights for multiple codes
for m = m1 : m2     %cycles through a range of m values
    figure();    %creates a new figure
    for r = r1 : r2   %cycles through a range of r values
        if r <= m
            n = 2^m;
            k = 0; %%initialize k = 0
            for i = 0 : r
                k = k + nchoosek(m,i);
            end
            rate = k/n; %calculats the rate of the code
            
            alphaValues = zeros(1,m);
            
            %determines a,b,c,ect. values
            for num = 1 : m
                alphaValues(1, num) = 2^(num-1);
            end
            
            finalValues = n;
            %calulates the weight heirarchy of RM code
            for numCombos = 1:r
                combos = nchoosek(alphaValues(1,:), numCombos);
                [rows columns] = size(combos);
                
                %sums up all of combinations
                for index = 1 : rows
                    comboSum = sum(combos(index,:));
                    finalValues = [finalValues 2^m-comboSum];
                end
            end
            
            
            %initialize variables
            weights = zeros(1, k);
            weightsIndex = 1;
            
            weights = sort(finalValues);
            
            %create y-axis values for all indexes
            graphWeights = zeros(1,n+1);
            for index = 1 : n + 1
                if index <= weights(1,weightsIndex)
                    graphWeights(1,index) = k - weightsIndex + 1;
                elseif index > weights(1, weightsIndex)
                    weightsIndex = weightsIndex + 1;
                    graphWeights(1,index) = k - weightsIndex + 1;
                end
            end
            
            %makes array for index of values
            graphIndexes = zeros(1,n+1);
            for j = 0 : n
                graphIndexes(1, j+1) = j;
            end
            
            %plots curves
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