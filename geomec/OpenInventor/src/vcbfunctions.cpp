#include "stdafx.h"

#include "vcbfunctions.h"
#include "Result.h"
#include "MeshResultTree.h"

namespace vcbfunctions
{
    /**
     * Builds a string containing the complete name of the given value component
     */
    QString getResultPath(const IValueComponentBase& valueComponent, int exceptFirst)
    {
        const IResultComponent *resultComponent = dynamic_cast<const IResultComponent*>(&valueComponent);
	
        if(!resultComponent)
        {
            const CMeshResult * meshResult = dynamic_cast<const CMeshResult *>(&valueComponent.Parent());
            if (exceptFirst >= 1 || !meshResult)
            {
                return valueComponent.Name();
            }
            else
            {
                QString ret;
                                  
                assert (meshResult);
                ret += meshResult->parent()->Name();
                ret += "::";
                ret += valueComponent.Name();

                return ret;
            }
        }

        // Get result of component
        const IResult& result = dynamic_cast<const IResult&>(valueComponent.Parent());
	
        std::vector<QString> path;

        // Is the result a scalar?
        if(result.ComponentSize() > 1)
            path.push_back(valueComponent.Name());

        path.push_back(result.Name());

        const CResultGroup *group = result.Parent();
        while(group)
        {
            path.push_back(group->Name());
            group = group->Parent();
        }

        QString ret;
        for(int i = path.size() - 1 - exceptFirst; i >= 0; --i)
        {
            if(ret.isEmpty())
                ret = path[i];
            else
                ret = ret + "::" + path[i];
        }

        if (resultComponent)
        {
          ret = ret + "_D" + QString::number(resultComponent->Stage().Index());
        }

        return ret;
    }
}
