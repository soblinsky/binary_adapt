const MeshBase & the_mesh = MeshOutput<MeshBase>::mesh();
dof_id_type n_active_elem = the_mesh.n_active_elem();
   70 
   71   if (this->mesh().processor_id() == 0)
   72     {
   73       std::stringstream data_stream_name;
   74       data_stream_name << fname << "_data";
   75       const std::string data_file_name = data_stream_name.str();
   76 
   77       // This class is designed only for use with 1D meshes
   78       libmesh_assert_equal_to (the_mesh.mesh_dimension(), 1);
   79 
   80       // Create an output stream for script file
   84       std::ofstream out_stream(fname.c_str());
   85 
   86       // Make sure it opened correctly
   87       if (!out_stream.good())
   88         libmesh_file_error(fname.c_str());
   89 
   94       // Write header to stream
   95       out_stream << "# This file was generated by gnuplot_io.C\n"
   96                  << "# Stores 1D solution data in GNUplot format\n"
   97                  << "# Execute this by loading gnuplot and typing "
   98                  << "\"call '" << fname << "'\"\n"
   99                  << "reset\n"
  100                  << "set title \"" << _title << "\"\n"
  101                  << "set xlabel \"x\"\n"
  102                  << "set xtics nomirror\n";
  103 
  104       // Loop over the elements to find the minimum and maximum x values,
  105       // and also to find the element boundaries to write out as xtics
  106       // if requested.
  107       Real x_min=0., x_max=0.;
  108 
  109       // construct string for xtic positions at element edges
  110       std::stringstream xtics_stream;
  111 
  112       MeshBase::const_element_iterator it = the_mesh.active_elements_begin();
  113       const MeshBase::const_element_iterator end_it =
  114         the_mesh.active_elements_end();
  115 
  116       unsigned int count = 0;
  117 
  118       for( ; it != end_it; ++it)
  119         {
  120           const Elem * el = *it;
  121 
  122           // if el is the left edge of the mesh, print its left node position
  123           if (el->neighbor_ptr(0) == libmesh_nullptr)
  124             {
  125               x_min = (el->point(0))(0);
  126               xtics_stream << "\"\" " << x_min << ", \\\n";
  127             }
  128           if (el->neighbor_ptr(1) == libmesh_nullptr)
  129             {
  130               x_max = (el->point(1))(0);
  131             }
  132           xtics_stream << "\"\" " << (el->point(1))(0);
  133 
  134           if(count+1 != n_active_elem)
  135             {
  136               xtics_stream << ", \\\n";
  137             }
  138           count++;
  139         }
  140 
  141       out_stream << "set xrange [" << x_min << ":" << x_max << "]\n";
  142 
  143       if(_grid)
  144         out_stream << "set x2tics (" << xtics_stream.str() << ")\nset grid noxtics noytics x2tics\n";
  145 
  146       if(_png_output)
  147         {
  148           out_stream << "set terminal png\n";
  149           out_stream << "set output \"" << fname << ".png\"\n";
  150         }
  151 
  152       out_stream << "plot "
  153                  << axes_limits
  154                  << " \"" << data_file_name << "\" using 1:2 title \""
 
  165       out_stream.close();
  166 
  167 
  168       // Create an output stream for data file
  169       std::ofstream data(data_file_name.c_str());
  170 
  171       if (!data.good())
  172         libmesh_error_msg("ERROR: opening output data file " << data_file_name);
  173 
  174       // get ordered nodal data using a map
  175       typedef std::pair<Real, std::vector<Number> > key_value_pair;
  176       typedef std::map<Real, std::vector<Number> > map_type;
  177       typedef map_type::iterator map_iterator;
  178 
  179       map_type node_map;
  180 
  181 
  182       it  = the_mesh.active_elements_begin();
  183 
  184       for ( ; it != end_it; ++it)
  185         {
  186           const Elem * elem = *it;
  187 
  188           for(unsigned int i=0; i<elem->n_nodes(); i++)
  189             {
  190               std::vector<Number> values;
  191 
  192               // Get the global id of the node
  193               dof_id_type global_id = elem->node_id(i);
  194 
  195 
  200               node_map[ the_mesh.point(global_id)(0) ] = values;
  201             }
  202         }
  203 
  204 
  205       map_iterator map_it = node_map.begin();
  206       const map_iterator end_map_it = node_map.end();
  207 
  208       for( ; map_it != end_map_it; ++map_it)
  209         {
  210           key_value_pair kvp = *map_it;
  211           std::vector<Number> values = kvp.second;
  212 
  213           data << kvp.first << "\t";
  214 
  215           for (std::size_t i=0; i<values.size(); i++)
  216             data << values[i] << "\t";
  217 
  218           data << "\n";
  219         }
  220 
  221       data.close();
  222     }