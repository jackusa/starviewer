/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientbrowsermenu.h"

#include <QVBoxLayout>
#include <QMouseEvent>

#include "patient.h"
#include "patientbrowsermenuextendeditem.h"
#include "patientbrowsermenulist.h"
#include "series.h"
#include "image.h"
#include "volume.h"
#include "volumerepository.h"
#include "applicationstylehelper.h"
#include "screenmanager.h"

namespace udg {

PatientBrowserMenu::PatientBrowserMenu(QWidget *parent) 
 : QWidget(parent), m_patientBrowserList(0), m_patientAdditionalInfo(0)
{
    ApplicationStyleHelper style;
    style.setScaledFontSizeTo(this);
}

PatientBrowserMenu::~PatientBrowserMenu()
{
}

void PatientBrowserMenu::setPatient(Patient *patient)
{
    createWidgets();
    QString caption;
    QString label;
    foreach (Study *study, patient->getStudies())
    {
        // Extreiem el caption de l'estudi
        caption = tr("Study %1 : %2 [%3] %4")
            .arg(study->getDateAsString())
            .arg(study->getTimeAsString())
            .arg(study->getModalitiesAsSingleString())
            .arg(study->getDescription());

        // Per cada sèrie de l'estudi extreurem el seu label i l'identificador
        QList<QPair<QString, QString> > itemsList;
        foreach (Series *series, study->getViewableSeries())
        {
            label = tr(" Serie %1: %2 %3 %4 %5")
                        .arg(series->getSeriesNumber().trimmed())
                        .arg(series->getProtocolName().trimmed())
                        .arg(series->getDescription().trimmed())
                        .arg(series->getBodyPartExamined())
                        .arg(series->getViewPosition());
            
            if (series->getNumberOfVolumes() > 1)
            {
                QString volumeID;
                int volumeNumber = 1;
                foreach (Volume *volume, series->getVolumesList())
                {
                    QPair<QString, QString> itemPair;
                    // Label
                    itemPair.first = label + " (" + QString::number(volumeNumber) + ")";
                    volumeNumber++;
                    // Identifier
                    itemPair.second = QString::number(volume->getIdentifier().getValue());
                    // Afegim el parell a la llista
                    itemsList << itemPair;
                }
            }
            else // Només tenim un sol volum per la sèrie
            {
                Volume *volume = series->getFirstVolume();
                QPair<QString, QString> itemPair;
                // Label
                itemPair.first = label;
                // Identifier
                itemPair.second = QString::number(volume->getIdentifier().getValue());
                // Afegim el parell a la llista
                itemsList << itemPair;
            }
        }
        // Afegim les sèries agrupades per estudi
        m_patientBrowserList->addItemsGroup(caption, itemsList);
    }

    connect(m_patientBrowserList, SIGNAL(isActive(QString)), SLOT(updateActiveItemView(QString)));
    connect(m_patientBrowserList, SIGNAL(selectedItem(QString)), SLOT(processSelectedItem(QString)));
}

void PatientBrowserMenu::updateActiveItemView(const QString &identifier)
{
    Identifier id(identifier.toInt());
    Volume *volume = VolumeRepository::getRepository()->getVolume(id);
    if (volume)
    {
        // Actualitzem les dades de l'item amb informació adicional
        m_patientAdditionalInfo->setPixmap(volume->getThumbnail());
        Series *series = volume->getImage(0)->getParentSeries();
        m_patientAdditionalInfo->setText(QString(tr("%1 \n%2 \n%3\n%4 Images"))
                                        .arg(series->getDescription().trimmed())
                                        .arg(series->getModality().trimmed())
                                        .arg(series->getProtocolName().trimmed())
                                        .arg(volume->getNumberOfFrames())
                                       );
        // Actualitzem la posició del widget amb la informació adicional
        updatePosition();
    }
}

void PatientBrowserMenu::popup(const QPoint &point, const QString &identifier)
{
    // Marquem l'ítem actual de la llista
    m_patientBrowserList->markItem(identifier);

    // Obtenim la geometria de la pantalla on se'ns ha demanat obrir el menú per posicionar correctament 
    // el widget per tal que no surti fora de la pantalla i no es pugui veure.
    ScreenManager screenManager;
    QRect currentScreenGeometry = screenManager.getAvailableScreenGeometry(screenManager.getScreenID(point));
    QPoint currentScreenGlobalOriginPoint = currentScreenGeometry.topLeft();

    // Calculem la mida total que faran els widgets
    QSize totalWidgetSize;
    totalWidgetSize.setWidth(m_patientBrowserList->sizeHint().width() + m_patientAdditionalInfo->sizeHint().width());
    totalWidgetSize.setHeight(qMax(m_patientBrowserList->sizeHint().height(),m_patientAdditionalInfo->sizeHint().height()));

    // Calculem les fronteres per on ens podria sortir el menú (lateral dret i per sota)
    int globalRight = currentScreenGlobalOriginPoint.x() + currentScreenGeometry.width();
    int globalBottom = currentScreenGlobalOriginPoint.y() + currentScreenGeometry.height();
    int widgetRight = totalWidgetSize.width() + point.x();
    int widgetBottom = totalWidgetSize.height() + point.y();

    // Calculem quant d'ample i/o alçada ens surt el menú. Si els valors són positius caldrà moure 
    // la posició original com a mínim tot el que ens sortim de les fronteres
    int outsideWidth = widgetRight - globalRight;
    int outsideHeight = widgetBottom - globalBottom;

    const int Margin = 5;
    int menuXPosition = point.x();
    int menuYPosition = point.y();
    if (outsideWidth > 0)
    {
        menuXPosition -= outsideWidth + Margin;
    }
    if (outsideHeight > 0)
    {
        menuYPosition -= outsideHeight + Margin;
    }

    // Movem la finestra del menu al punt que toca
    m_patientBrowserList->move(menuXPosition, menuYPosition);
    m_patientBrowserList->show();
    // Col·loquem el widget amb la informació adicional a la dreta del principal
    m_patientAdditionalInfo->move(menuXPosition + m_patientBrowserList->sizeHint().width(), m_patientBrowserList->y());
    m_patientAdditionalInfo->show();
    // TODO No es té en compte si després d'haver mogut de lloc el widget aquest ja es veu correctament, 
    // ja que es podria donar el cas que el widget no hi cabés a tota la pantalla
    // Caldria millorar el comportament en certs aspectes, com per exemple, redistribuir les files i columnes
    // del llistat si aquest no hi cap a la pantalla, per exemple.
}

void PatientBrowserMenu::processSelectedItem(const QString &identifier)
{
    m_patientAdditionalInfo->hide();
    m_patientBrowserList->hide();

    if (m_patientBrowserList->getMarkedItem() != identifier)
    {
        Identifier id(identifier.toInt());
        emit selectedVolume(VolumeRepository::getRepository()->getVolume(id));
    }
}

void PatientBrowserMenu::updatePosition()
{
    m_patientAdditionalInfo->move(m_patientBrowserList->x() + m_patientBrowserList->sizeHint().width(), m_patientBrowserList->y());
    m_patientAdditionalInfo->show();
}

void PatientBrowserMenu::createWidgets()
{
    if (m_patientAdditionalInfo)
    {
        delete m_patientAdditionalInfo;
    }
    
    if (m_patientBrowserList)
    {
        delete m_patientBrowserList;
    }
    
    m_patientAdditionalInfo = new PatientBrowserMenuExtendedItem(this);
    m_patientBrowserList = new PatientBrowserMenuList(this);

    m_patientAdditionalInfo->setWindowFlags(Qt::Popup);
    m_patientBrowserList->setWindowFlags(Qt::Popup);
    
    connect(m_patientAdditionalInfo, SIGNAL(close()), m_patientBrowserList, SLOT(close()));
    connect(m_patientBrowserList, SIGNAL(close()), m_patientAdditionalInfo, SLOT(close()));
}

}
