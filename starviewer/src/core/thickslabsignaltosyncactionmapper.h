#ifndef THICKSLABSIGNALTOSYNCACTIONMAPPER_H
#define THICKSLABSIGNALTOSYNCACTIONMAPPER_H

#include "signaltosyncactionmapper.h"

// Includes needed to do the factory register
#include "signaltosyncactionmapperfactoryregister.h"
#include "thickslabsyncaction.h"

namespace udg {

/**
    Implementation of SignalToSyncActionMapper to map thick slab changes to ThicSlabSyncAction.
 */
class ThickSlabSignalToSyncActionMapper : public SignalToSyncActionMapper {

    Q_OBJECT

public:
    ThickSlabSignalToSyncActionMapper(QObject *parent = 0);
    virtual ~ThickSlabSignalToSyncActionMapper();

protected:
    /// Maps the thick slab signals from the viewer to the actionMapped(SyncAction*) signal.
    virtual void mapSignal();

    /// Unmaps the thick slab signals from the viewer to the actionMapped(SyncAction*) signal.
    virtual void unmapSignal();

private slots:
    /// Updates the mapped sync action with the given slab projection mode and the viewer's main volume and emits the actionMapped(SyncAction*) signal.
    void mapProjectionModeToSyncAction(int slabProjectionMode);
    /// Updates the mapped sync action with the given slab thickness and the viewer's main volume and emits the actionMapped(SyncAction*) signal.
    void mapThicknessToSyncAction(int slabThickness);

};

// With this line we register this mapper on the factory
static SignalToSyncActionMapperFactoryRegister<ThickSlabSignalToSyncActionMapper> registerThickSlabSignalToSyncActionMapper(ThickSlabSyncAction().getMetaData());

}

#endif