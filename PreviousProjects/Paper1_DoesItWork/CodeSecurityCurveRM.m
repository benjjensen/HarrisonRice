%Created by Bradford Clark % Used by Benjamin Jensen
%This code calculates security curves for different Reed-Muller (RM) codes.
%closes all figure windows
close all;
for m = 2 : 10   %cycles through a range of m values
    %creates a new figure
    figure();
    for r = 1 : m   %cycles through a range of r values
        if r < m    %RM codes are only valid if this is true.
            n = 2^m;    %length of code words
            k = 0;  %initialize k = 0
            
            %computes k
            for i = 0 : r
                k = k + nchoosek(m,i);
            end
            rate = k/n; %rate of the code
            
            alphaValues = zeros(1,m);
            
            %determines a,b,c,ect. values
            for num = 1 : m
                alphaValues(1, num) = 2^(num-1);
            end
            
            finalValues = n;
            
            %calulates the weight heirarchy of RM code
            for numCombos = 1:r
                combos = nchoosek(alphaValues(1,:), numCombos); %finds possilbe combos of matrix
                [rows, columns] = size(combos);
                
                %sums up all of combinations
                for index = 1 : rows
                    comboSum = sum(combos(index,:));
                    finalValues = [finalValues 2^m-comboSum];   %merges matrix's
                end
            end
            
            %initialize variables
            weightsIndex = 1;
            graphWeights = zeros(1,n+1);
            
            weights = sort(finalValues);    %orders numbers from high to low
            
            %create y-axis values for all indexes
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
            plot(graphIndexes,graphWeights,'DisplayName',['RM(' num2str(r) ...
                ',' num2str(m) '), k = ' num2str(k) ', rate = ' num2str(round(rate,2))]);
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