#include "ModelBase.h"
#include "NewWellPathInput.h"
#include "NewWellPath.h"
#include "BoundingBox.h"

#include "WP_Helper.h"

//#include "Printer.h"

/*void WP_Helper::ps_wp( CPointSetEntry* ps )
{
	const CPointSetEntry::TNodeSet& stNodes = ps->EntryNodes();
	Printer::instance()->debug("ps : input : INI");
	for (CPointSetEntry::TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it)
		if ((*it)->pointSetType() == IPointSet::INPUT)
			Printer::instance()->debug(" node: %s", (*it)->Name().toStdString().c_str());
	Printer::instance()->debug("ps : input : END");
}*/

/*void WP_Helper::model_wp( CPointSetEntry* ps )
{
	CModelBase& rootModel = static_cast<CModelBase&>( ps->Model() ).RootModel();

	CNewWellPathEntry* well_entry = dynamic_cast<CNewWellPathEntry*>( rootModel.GraphEntry(MD_NEW_WELLPATH) );

	const CNewWellPathEntry::TNodeSet& wp_s = well_entry->EntryNodes();
	Printer::instance()->debug("model : wp : INI");
	for (CNewWellPathEntry::TNodeSet::iterator it = wp_s.begin(); it != wp_s.end(); ++it)
		Printer::instance()->debug(" wp: %s", (*it)->Name().toStdString().c_str() );
	Printer::instance()->debug("model : wp : END");
}*/

bool WP_Helper::is_present_ps( std::vector< CNewWellPathInput* >& inputs_v, CNewWellPathInput* input )
{
	bool res = false;
	for( int i = 0; i < inputs_v.size() && !res; i++ )
	{
		if( *inputs_v[ i ] == *input )
			res = true;
	}
	//Printer::instance()->debug("present : %s : size:%d res:%d", input->Name().toStdString().c_str() , inputs_v.size(), res);
	return res;
}

bool WP_Helper::is_present_in_model( CNewWellPathInput* input )
{
	bool res = false;
	CModelBase& rootModel = static_cast<CModelBase&>( m_ps->Model() ).RootModel();

	CNewWellPathEntry* well_entry = dynamic_cast<CNewWellPathEntry*>( rootModel.GraphEntry(MD_NEW_WELLPATH) );

	const CNewWellPathEntry::TNodeSet& wp_s = well_entry->EntryNodes();
	for( CNewWellPathEntry::TNodeSet::iterator it = wp_s.begin(); it != wp_s.end() && !res ; ++it )
	{
		const CNewWellPathInput* input_ = (*it)->WellPathInput();
		if( *input_ == *input )
				res = true;
		//Printer::instance()->debug(" wp: %s", (*it)->Name().toStdString().c_str() );
	}
	return res;
}

void WP_Helper::remove_inputs( std::vector< CNewWellPathInput* > inputs_v )
{
	//Printer::instance()->debug("remove_inputs: %dx", inputs_v.size() );
	for( int i = 0; i < inputs_v.size(); i++ )
		inputs_v[ i ]->Destroy();
}

std::vector< CNewWellPathInput* > WP_Helper::duplicates_ps_wp()
{
	std::vector< CNewWellPathInput * > inputs_v;
	std::vector< CNewWellPathInput * > inputs_to_remove_v;

	const CPointSetEntry::TNodeSet& stNodes = m_ps->EntryNodes();
	//Printer::instance()->debug("ps : input : fix : INI");
	for (CPointSetEntry::TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it)
	{
		if ((*it)->pointSetType() == IPointSet::INPUT)
		{
			CNewWellPathInput* input = static_cast<CNewWellPathInput *>(*it);

			//Printer::instance()->debug(" node: %s", input->Name().toStdString().c_str());

			if( is_present_ps( inputs_v, input ) )
			{
				inputs_to_remove_v.push_back( input );
			}
			else
			{
				inputs_v.push_back( input );
			}
		}
	}
	//remove_inputs( inputs_to_remove_v );
	//Printer::instance()->debug("ps : input : fix : END");
	return inputs_to_remove_v;
}

bool WP_Helper::is_in_box( CModelBase& rootModel, CNewWellPathInput* input )
{
	geo::CPoint min = rootModel.Mesh().Min();
	geo::CPoint max = rootModel.Mesh().Max();

	bool res = geo::BBox::Intersects( min, max, input->min(), input->max(), true );
	return res;
}

void WP_Helper::CopyAllWellpaths()
{
	//Printer::instance()->debug("-> CopyAllWellpaths");
	CModelBase& rootModel = static_cast<CModelBase&>(m_ps->Model()).RootModel();
	CPointSetEntry::TNodeSet& stNodes = (CPointSetEntry::TNodeSet&)(m_ps->EntryNodes());
	//Printer::instance()->debug("CREATE 0");
	//wp::print::ps_wp( ps );
	remove_inputs( duplicates_ps_wp() );
	//wp::print::ps_wp( ps );
	//wp::print::model_wp( ps );
	std::vector< CNewWellPathInput * > inputs_v;
	CGraphNode::TNodeVec nodeVec;

	//std::vector< CNewWellPathInput* > inputs_to_remove_v;

	if (!rootModel.CanCreateMesh())
	{
		for (CPointSetEntry::TNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it)
		{
			if ((*it)->pointSetType() == IPointSet::INPUT)
			{
				//Printer::instance()->debug("ps: %s", (*it)->Name().toStdString().c_str());
				CNewWellPathInput* input = static_cast<CNewWellPathInput *>(*it);

				bool in_model = is_present_in_model( input );
				bool in_ps = is_present_ps( inputs_v, input ); // This shouldn't happen -> teatred before

				//Printer::instance()->debug("in_ps: %d | in_model: %d", in_ps, in_model );
				if( in_model )
				{
					//Printer::instance()->debug("in_model so nothing to do");
				}
				else
				{
					//Printer::instance()->debug("not in_model");

					if( rootModel.IsMesh() )
					{
						if( is_in_box( rootModel, input ) )
						{
							CNewWellPath *pWellPath = new CNewWellPath( *input, const_cast<CModelBase&>( rootModel ) );

							if (pWellPath)
							{
								inputs_v.push_back( input );
								//Printer::instance()->debug("-> wp created (from ps): %s", pWellPath->Name().toStdString().c_str());
								nodeVec.push_back( const_cast<CNewWellPath *>( pWellPath ) );
							}
							else
							{
								//Printer::instance()->debug("-> wp not created");	// Never happens
							}
						}
					}
				}
			}
		}
		//wp::remove_inputs( inputs_to_remove_v );
		CNewWellPathEntry::addPointSet2DataStorage(nodeVec, rootModel);
	}

	//Printer::instance()->debug("END");

	//wp::print::ps_wp( ps );
	//wp::print::model_wp( ps );

	//Printer::instance()->debug("<- CopyAllWellpaths");
}