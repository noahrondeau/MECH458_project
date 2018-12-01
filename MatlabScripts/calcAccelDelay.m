function delay = calcAccelDelay(dist,lastDelay)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
minDist = 1;
maxDist = 100;
minDelay = 8;
maxDelay = 20;
distRow = transpose(minDist:1:maxDist);
lastDelayCol = 1:1:maxDelay+1;
delayMatrix = zeros(length(distRow),length(lastDelayCol));

for i=1:length(distRow)
    for j=1:length(lastDelayCol)
        if distRow(i) > (maxDelay - minDelay)
            if lastDelayCol(j) > minDelay
                delayMatrix(i,j) = lastDelayCol(j) - 1;
            else
                delayMatrix(i,j) = minDelay;
            end
        else
            if lastDelayCol(j) <= (maxDelay - distRow(i))
                delayMatrix(i,j) = lastDelayCol(j) + 1;
            else
                delayMatrix(i,j) = lastDelayCol(j) - 1;
            end
        end
    end
end

delay = delayMatrix(dist, lastDelay);

end

