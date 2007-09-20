/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGEXTENSIONCONTEXT_H
#define UDGEXTENSIONCONTEXT_H

#include <QStringList>

namespace udg {

class Patient;
class Volume;

/**
Contexte on s'executa una extensió. Permet accedir a informació que pot ser útil a l'extensió com l'objecte Patient, la finestra on s'executa, els "readers"..

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ExtensionContext{
public:
    ExtensionContext();

    ~ExtensionContext();

    /// get/set del pacient amb el que ha de treballar l'extensió.
    Patient* getPatient() const;
    void setPatient(Patient *patient);

    /**
     * Li assignem quins són els estudis que es volen visualitzar per defecte
     * dins de l'input de pacient que donem
     * @param selectedStudies Llista dels UIDs dels estudis que volem tenir
     * seleccionats per defecte
     */
    void setDefaultSelectedStudies( QStringList selectedStudies );

    /**
     * Afegim un estudi més per defecte a la llista
     * @param study L'uid de l'estudi
     */
    void addDefaultSelectedStudy( QString study );
    void addDefaultSelectedStudies( QStringList studies );

    /**
     * Retorna la llista d'estudis seleccionats per defecte
     * @return La llista amb els UID dels estudis seleccionats per defecte
     */
    QStringList getDefaultSelectedStudies() const;

    /**
     * Li assignem quines són les sèries que es volen visualitzar per defecte
     * dins de l'input de pacient que donem
     * @param selectedSeries Llista dels UIDs de les sèries que volem tenir
     * seleccionades per defecte
     */
    void setDefaultSelectedSeries( QStringList selectedSeries );

    /**
     * Afegim una sèrie més per defecte a la llista
     * @param series L'uid de la sèrie
     */
    void addDefaultSelectedSeries( QString series );
    void addDefaultSelectedSeries( QStringList series );

    /**
     * Retorna la llista de series seleccionades per defecte
     * @return La llista amb els UID de les series seleccionades per defecte
     */
    QStringList getDefaultSelectedSeries() const;

    /// Mètode de conveniència que ens dóna un volum per defecte a partir del contexte assignat.
    /// Això ens servirà per obtenir un volum a partir tant del pacient o si no
    /// tenim pacient (cas mhd's) fer-ho amb el volumeIdentifier
    Volume *getDefaultVolume() const;

private:
    Patient* m_patient;

    QStringList m_defaultSelectedStudies;
    QStringList m_defaultSelectedSeries;
};

}

#endif
